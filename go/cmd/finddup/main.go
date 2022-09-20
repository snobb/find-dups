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
	"runtime"
	"runtime/trace"
	"sort"
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

func walkDirs(dirs []string) <-chan string {
	// start sending files to the processing fileCh
	fileCh := make(chan string, 1)

	go func() {
		defer close(fileCh)

		for _, dir := range dirs {
			walk(dir, fileCh)
		}
	}()

	return fileCh
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

		sort.Strings(files)

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

func main() {
	var (
		nworker int
		isVer   bool
		isJSON  bool
		isTrace bool
	)

	// handle command arguments
	flag.IntVar(&nworker, "n", runtime.NumCPU(), "number of workers")
	flag.BoolVar(&isVer, "v", false, "show version")
	flag.BoolVar(&isJSON, "j", false, "output json")
	flag.BoolVar(&isTrace, "t", false, "save trace data to trace.out")
	flag.Parse()

	if isVer {
		fmt.Println(version)
		return
	}

	if isTrace || os.Getenv("TRACE") != "" {
		f, err := os.Create("trace.out")
		if err != nil {
			panic(err)
		}

		if err := trace.Start(f); err != nil {
			panic(err)
		}
		defer trace.Stop()
	}

	dirs := flag.Args()
	if len(dirs) == 0 {
		dirs = []string{"."} // default to current dir
	}

	var (
		cnt uint64
		wg  sync.WaitGroup
	)

	// handle results in a separate go routine
	resultCh := make(chan *fileInfo, nworker)
	taskCh := make(chan string, nworker)
	registry := make(map[string][]string)

	// start result handler worker.
	go func() {
		for res := range resultCh {
			cnt++
			fmt.Fprintf(os.Stderr, "Processed: %d\r", cnt)
			registry[res.hash] = append(registry[res.hash], res.file)
			wg.Done()
		}
	}()

	// start <nworker> workers.
	for i := 0; i < nworker; i++ {
		go func() {
			for file := range taskCh {
				res, err := handleFile(file)
				if err != nil {
					fmt.Fprintf(os.Stderr, "Error: %s\n", err.Error())
				}

				resultCh <- res
			}
		}()
	}

	// walk files and get a channel with files.
	for file := range walkDirs(dirs) {
		wg.Add(1)
		taskCh <- file
	}

	wg.Wait()
	close(taskCh)
	close(resultCh)

	// print results
	if isJSON {
		printJSON(registry)
	} else {
		printText(registry)
	}
}
