#!/bin/bash
protoc --proto_path=../proto/ --python_out=pb ../proto/aff3ct.proto
