# protobuf build proto file to *.c *.h

[nanopb](https://github.com/nanopb/nanopb#readme) 

[nanopb api](https://jpa.kapsi.fi/nanopb/docs/reference.html#proto-file-options)

## python cmd build

> [@ref](https://jpa.kapsi.fi/nanopb/docs/concepts.html#modifying-generator-behaviour)

* 安装python
* python 安装依赖包 pip install --upgrade protobuf grpcio-tools
* cd app/nanopb
* python generator/nanopb_generator.py simple.proto

## binary packages build

* download nanopd release packet [download](https://jpa.kapsi.fi/nanopb/download/)
* generator-bin/nanopb_generator myprotocol.proto
