import json
import os
import argparse
from pathlib import Path
from msggen.gen.grpc import GrpcGenerator, GrpcConverterGenerator, GrpcUnconverterGenerator, GrpcServerGenerator
from msggen.gen.grpc2py import Grpc2PyGenerator
from msggen.gen.rust import RustGenerator
from msggen.gen.generator import GeneratorChain
from msggen.utils import load_jsonrpc_service
import logging
from msggen.patch import VersionAnnotationPatch, OptionalPatch
from msggen.checks import VersioningCheck


logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.StreamHandler()
    ]
)


def add_handler_gen_grpc(generator_chain: GeneratorChain, meta):
    """Load all mapped RPC methods, wrap them in a Service, and split them into messages.
    """
    fname = Path("cln-grpc") / "proto" / "node.proto"
    dest = open(fname, "w")
    generator_chain.add_generator(GrpcGenerator(dest, meta))

    fname = Path("cln-grpc") / "src" / "convert.rs"
    dest = open(fname, "w")
    generator_chain.add_generator(GrpcConverterGenerator(dest))
    generator_chain.add_generator(GrpcUnconverterGenerator(dest))

    fname = Path("cln-grpc") / "src" / "server.rs"
    dest = open(fname, "w")
    generator_chain.add_generator(GrpcServerGenerator(dest))


def add_handler_get_grpc2py(generator_chain: GeneratorChain):
    fname = Path("contrib") / "pyln-testing" / "pyln" / "testing" / "grpc2py.py"
    dest = open(fname, "w")
    generator_chain.add_generator(Grpc2PyGenerator(dest))


def add_handler_gen_rust_jsonrpc(generator_chain: GeneratorChain):
    fname = Path("cln-rpc") / "src" / "model.rs"
    dest = open(fname, "w")
    generator_chain.add_generator(RustGenerator(dest))


def load_msggen_meta():
    meta = json.load(open('.msggen.json', 'r'))
    return meta


def write_msggen_meta(meta):
    pid = os.getpid()
    with open(f'.msggen.json.tmp.{pid}', 'w') as f:
        json.dump(meta, f, sort_keys=True, indent=4)
    os.rename(f'.msggen.json.tmp.{pid}', '.msggen.json')


def run(rootdir: Path):
    schemadir = rootdir / "doc" / "schemas"
    meta = load_msggen_meta()
    service = load_jsonrpc_service(
        schema_dir=schemadir,
    )

    p = VersionAnnotationPatch(meta=meta)
    p.apply(service)
    OptionalPatch().apply(service)

    # Run the checks here, we should eventually split that out to a
    # separate subcommand
    VersioningCheck().check(service)
    generator_chain = GeneratorChain()

    add_handler_gen_grpc(generator_chain, meta)
    add_handler_gen_rust_jsonrpc(generator_chain)
    add_handler_get_grpc2py(generator_chain)

    generator_chain.generate(service)

    write_msggen_meta(meta)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--rootdir',
        dest='rootdir',
        default='.'
    )
    args = parser.parse_args()
    run(
        rootdir=Path(args.rootdir)
    )
