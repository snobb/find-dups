export GO111MODULE=on
export GOVCS=*:git

TARGET   := finddup
NAME     := finddup
MAIN     := ./cmd/finddup/main.go
BIN      := ./bin
BRANCH   := ${shell git rev-parse --abbrev-ref HEAD}
REVCNT   := ${shell git rev-list --count $(BRANCH)}
REVHASH  := ${shell git log -1 --format="%h"}
LDFLAGS  := -X main.version=${BRANCH}.${REVCNT}.${REVHASH}
CFLAGS   := --ldflags '${LDFLAGS}' -o $(BIN)/$(TARGET)

all: lint build

lint:
	golangci-lint run

build:
	go build ${CFLAGS} $(MAIN)

build-linux: clean
	CGO_ENABLED=0 GOOS=linux go build ${CFLAGS} -a -installsuffix cgo $(MAIN)

build-rpi:
	GOOS=linux GOARCH=arm GOARM=5 go build -o $(BIN)/$(TARGET) $(MAIN)

clean:
	-rm -rf $(BIN)

.PHONY: build
