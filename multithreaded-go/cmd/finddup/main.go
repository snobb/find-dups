package main

import (
	"crypto/md5"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"sync"
)

var version string

type fileInfo struct {
	file string
	hash string
}

type resJSON struct {
	Hash  string   `json:"hash"`
	Files []string `json:"files"`
}

func walk(dir string, queue chan<- string) {
	err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Fprintln(os.Stderr, "ERROR:", err)
			return err
		}

		if info.Mode().IsRegular() {
			queue <- path
		}

		return nil
	})

	if err != nil {
		log.Fatal(err)
	}
}

func handleFile(file string) (*fileInfo, error) {
	hash := md5.New()
	defer hash.Reset()

	fh, err := os.Open(file)
	if err != nil {
		return nil, err
	}
	defer fh.Close()

	if _, err := io.Copy(hash, fh); err != nil {
		return nil, err
	}

	return &fileInfo{
		file: file,
		hash: fmt.Sprintf("%x", hash.Sum(nil)),
	}, nil
}

func printText(reg map[string][]string) {
	found := false

	for hash, files := range reg {
		if len(files) > 1 {
			found = true
			fmt.Println(hash)

			for _, file := range files {
				fmt.Printf("\t\"%s\"\n", file)
			}
		}
	}

	if !found {
		fmt.Println()
	}
}

func printJSON(reg map[string][]string) {
	var res []resJSON

	for hash, files := range reg {
		if len(files) == 1 {
			continue
		}

		res = append(res, resJSON{
			Hash:  hash,
			Files: files,
		})
	}

	if len(res) == 0 {
		return
	}

	if err := json.NewEncoder(os.Stdout).Encode(res); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %s\n", err.Error())
	}
}

func walkDirs(dirs []string) <-chan string {
	// start sending files to the processing fileCh
	fileCh := make(chan string)
	go func() {
		defer close(fileCh)

		for _, dir := range dirs {
			walk(dir, fileCh)
		}
	}()

	return fileCh
}

func main() {
	var (
		nworker int
		ver     bool
		outJSON bool
	)

	// handle command arguments
	flag.IntVar(&nworker, "n", 4, "number of workers")
	flag.BoolVar(&ver, "v", false, "show version")
	flag.BoolVar(&outJSON, "j", false, "output json")
	flag.Parse()

	if ver {
		fmt.Println(version)
		return
	}

	registry := make(map[string][]string)

	// handle results in a separate go routine
	resultCh := make(chan *fileInfo)
	var cnt uint64
	var wg sync.WaitGroup

	go func() {
		for res := range resultCh {
			cnt++
			fmt.Fprintf(os.Stderr, "Processing file: %d\r", cnt)
			registry[res.hash] = append(registry[res.hash], res.file)
			wg.Done()
		}
	}()

	taskCh := make(chan struct{}, nworker)

	dirs := flag.Args()
	if len(dirs) == 0 {
		dirs = []string{"."} // default to current dir
	}

	// walk files and get a channel with files.
	for file := range walkDirs(dirs) {
		taskCh <- struct{}{}
		wg.Add(1)

		go func(file string) {
			defer func() {
				<-taskCh
			}()

			res, err := handleFile(file)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Error: %s\n", err.Error())
				return
			}

			resultCh <- res
		}(file)
	}

	wg.Wait()

	if outJSON {
		printJSON(registry)
	} else {
		printText(registry)
	}
}
