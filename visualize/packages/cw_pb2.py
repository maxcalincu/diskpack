# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# NO CHECKED-IN PROTOBUF GENCODE
# source: cw.proto
# Protobuf Python Version: 5.30.0-dev
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    5,
    29,
    0,
    '',
    'cw.proto'
)
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x08\x63w.proto\x12\x03\x43\x44P\"?\n\x04\x44isk\x12\t\n\x01x\x18\x01 \x01(\x01\x12\t\n\x01y\x18\x02 \x01(\x01\x12\x0e\n\x06radius\x18\x03 \x01(\x01\x12\x11\n\tsize_type\x18\x04 \x01(\x05\"7\n\x07Packing\x12\x12\n\ndimensions\x18\x01 \x01(\x05\x12\x18\n\x05\x64isks\x18\x02 \x03(\x0b\x32\t.CDP.Diskb\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'cw_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_DISK']._serialized_start=17
  _globals['_DISK']._serialized_end=80
  _globals['_PACKING']._serialized_start=82
  _globals['_PACKING']._serialized_end=137
# @@protoc_insertion_point(module_scope)
