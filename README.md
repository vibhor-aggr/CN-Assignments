# Computer Networks Assignments

Coursework repository for computer networks assignments covering written
networking problems, TCP stream components, and concurrent socket-server
experiments.

## Repository Layout

- `Assignment1/` contains the assignment brief and solution report.
- `Assignment2/` contains C++ networking components and tests:
  - byte stream implementation,
  - stream reassembler,
  - TCP receiver,
  - wrapping integer utilities,
  - CMake test targets.
- `Assignment3/` contains C socket-server experiments using `select`, `poll`,
  and `epoll`, along with a Makefile, analysis script, client archive, report,
  and setup README.

## Requirements

- C and C++ compiler toolchains.
- CMake for `Assignment2`.
- Make for `Assignment3`.
- A Unix-like environment is recommended for socket and event-loop experiments.

## Usage

For `Assignment2`, use the provided CMake files in the source and test
directories. For `Assignment3`, enter the assignment directory and use the
provided `Makefile`.

Read each assignment PDF and report for expected behavior, test setup, and
analysis details before running experiments.

## Notes

Assignment-specific setup instructions for the socket-server experiment are in
`Assignment3/README.md`.
