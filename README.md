## Introduction

The application allows users to store RSS feeds on proxy server and view/manage them from the client reader app. It provides features for adding new feeds, viewing them filtered by labels, and retrieving information about the feeds.

## Features

- Add new RSS feeds by providing the URL.
- Assign labels to feeds for better organization.
- Get detailed information about stored feeds, including its last synchronization time, potential synchronization errors.
- Fetch and display the actual content of a specific feed.

## Prerequisites

- C++ compiler
- CMake
- Qt framework (minimum version 6.6.1)

## Installation

```
git clone https://github.com/cvldrt/rss-feed-manager.git
cd rss-feed-manager
mkdir build
cd build
cmake ..
make
```

## Usage
```
./server [<path>]
./client [<path>]
```

Where \<path> is a directory where feeds/temporary images will be stores. Defaults are "./.feeds" and "./.imgs" respectively.
