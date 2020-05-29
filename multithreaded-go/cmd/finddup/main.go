package main

import (
	"crypto/md5"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sync"
)

type result struct {
	file string
	hash string
}

func walk(dir string, queue chan<- string) {
	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Fprintln(os.Stderr, "ERROR:", err)

		} else if info.Mode().IsRegular() {
			queue <- path
		}

		return nil
	})
}

func worker(queue <-chan string, res chan<- *result, wg *sync.WaitGroup) {
	h := md5.New()

	for file := range queue {
		f, err := os.Open(file)
		if err != nil {
			fmt.Fprintln(os.Stderr, "ERROR:", err)
		}

		if _, err := io.Copy(h, f); err != nil {
			fmt.Fprintln(os.Stderr, "ERROR:", err)
			continue
		}
		f.Close()

		res <- &result{
			file: file,
			hash: fmt.Sprintf("%x", h.Sum(nil)),
		}

		h.Reset()
	}

	wg.Done()
}

func main() {
	var nworker int

	// handle command arguments
	flag.IntVar(&nworker, "n", 6, "number of workers")
	flag.Parse()

	dirs := flag.Args()

	if len(dirs) == 0 {
		dirs = []string{"."}
	}

	// start sending files to the processing queue
	queue := make(chan string)
	go func() {
		for _, dir := range dirs {
			walk(dir, queue)
		}
		close(queue)
	}()

	registry := make(map[string][]string)
	results := make(chan *result)

	// handle results in a separate go routine
	done := make(chan struct{})
	go func() {
		var cnt int64 = 1
		for res := range results {
			fmt.Fprintf(os.Stderr, "\rProcessing file: %d ", cnt)
			registry[res.hash] = append(registry[res.hash], res.file)
			cnt++
		}
		close(done)
	}()

	// start workers
	var wg sync.WaitGroup
	wg.Add(nworker)

	for i := 0; i < nworker; i++ {
		go worker(queue, results, &wg)
	}
	wg.Wait()
	close(results)
	<-done

	// output results
	gotDups := false
	for hash, files := range registry {
		if len(files) > 1 {
			gotDups = true

			fmt.Println(hash)
			for _, file := range files {
				fmt.Printf("\t\"%s\"", file)
			}
		}
	}

	if !gotDups {
		fmt.Println()
	}
}
