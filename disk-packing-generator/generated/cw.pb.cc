// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protos/cw.proto

#include "cw.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_protos_2fcw_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_ProtoDisk_protos_2fcw_2eproto;
namespace CDP {
class ProtoDiskDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ProtoDisk> _instance;
} _ProtoDisk_default_instance_;
class ProtoPackingDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ProtoPacking> _instance;
} _ProtoPacking_default_instance_;
}  // namespace CDP
static void InitDefaultsscc_info_ProtoDisk_protos_2fcw_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::CDP::_ProtoDisk_default_instance_;
    new (ptr) ::CDP::ProtoDisk();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::CDP::ProtoDisk::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_ProtoDisk_protos_2fcw_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_ProtoDisk_protos_2fcw_2eproto}, {}};

static void InitDefaultsscc_info_ProtoPacking_protos_2fcw_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::CDP::_ProtoPacking_default_instance_;
    new (ptr) ::CDP::ProtoPacking();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::CDP::ProtoPacking::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_ProtoPacking_protos_2fcw_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_ProtoPacking_protos_2fcw_2eproto}, {
      &scc_info_ProtoDisk_protos_2fcw_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_protos_2fcw_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_protos_2fcw_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_protos_2fcw_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_protos_2fcw_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoDisk, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoDisk, x_),
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoDisk, y_),
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoDisk, radius_),
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoDisk, size_type_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoPacking, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoPacking, dimensions_),
  PROTOBUF_FIELD_OFFSET(::CDP::ProtoPacking, disks_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::CDP::ProtoDisk)},
  { 9, -1, sizeof(::CDP::ProtoPacking)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::CDP::_ProtoDisk_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::CDP::_ProtoPacking_default_instance_),
};

const char descriptor_table_protodef_protos_2fcw_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\017protos/cw.proto\022\003CDP\"D\n\tProtoDisk\022\t\n\001x"
  "\030\001 \001(\001\022\t\n\001y\030\002 \001(\001\022\016\n\006radius\030\003 \001(\001\022\021\n\tsiz"
  "e_type\030\004 \001(\005\"A\n\014ProtoPacking\022\022\n\ndimensio"
  "ns\030\001 \001(\005\022\035\n\005disks\030\002 \003(\0132\016.CDP.ProtoDiskb"
  "\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_protos_2fcw_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_protos_2fcw_2eproto_sccs[2] = {
  &scc_info_ProtoDisk_protos_2fcw_2eproto.base,
  &scc_info_ProtoPacking_protos_2fcw_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_protos_2fcw_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_protos_2fcw_2eproto = {
  false, false, descriptor_table_protodef_protos_2fcw_2eproto, "protos/cw.proto", 167,
  &descriptor_table_protos_2fcw_2eproto_once, descriptor_table_protos_2fcw_2eproto_sccs, descriptor_table_protos_2fcw_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_protos_2fcw_2eproto::offsets,
  file_level_metadata_protos_2fcw_2eproto, 2, file_level_enum_descriptors_protos_2fcw_2eproto, file_level_service_descriptors_protos_2fcw_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_protos_2fcw_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_protos_2fcw_2eproto)), true);
namespace CDP {

// ===================================================================

void ProtoDisk::InitAsDefaultInstance() {
}
class ProtoDisk::_Internal {
 public:
};

ProtoDisk::ProtoDisk(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:CDP.ProtoDisk)
}
ProtoDisk::ProtoDisk(const ProtoDisk& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&x_, &from.x_,
    static_cast<size_t>(reinterpret_cast<char*>(&size_type_) -
    reinterpret_cast<char*>(&x_)) + sizeof(size_type_));
  // @@protoc_insertion_point(copy_constructor:CDP.ProtoDisk)
}

void ProtoDisk::SharedCtor() {
  ::memset(&x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&size_type_) -
      reinterpret_cast<char*>(&x_)) + sizeof(size_type_));
}

ProtoDisk::~ProtoDisk() {
  // @@protoc_insertion_point(destructor:CDP.ProtoDisk)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void ProtoDisk::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void ProtoDisk::ArenaDtor(void* object) {
  ProtoDisk* _this = reinterpret_cast< ProtoDisk* >(object);
  (void)_this;
}
void ProtoDisk::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void ProtoDisk::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ProtoDisk& ProtoDisk::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ProtoDisk_protos_2fcw_2eproto.base);
  return *internal_default_instance();
}


void ProtoDisk::Clear() {
// @@protoc_insertion_point(message_clear_start:CDP.ProtoDisk)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&size_type_) -
      reinterpret_cast<char*>(&x_)) + sizeof(size_type_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* ProtoDisk::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // double x = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 9)) {
          x_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double y = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          y_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double radius = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 25)) {
          radius_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // int32 size_type = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          size_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ProtoDisk::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:CDP.ProtoDisk)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // double x = 1;
  if (!(this->x() <= 0 && this->x() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(1, this->_internal_x(), target);
  }

  // double y = 2;
  if (!(this->y() <= 0 && this->y() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(2, this->_internal_y(), target);
  }

  // double radius = 3;
  if (!(this->radius() <= 0 && this->radius() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(3, this->_internal_radius(), target);
  }

  // int32 size_type = 4;
  if (this->size_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_size_type(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:CDP.ProtoDisk)
  return target;
}

size_t ProtoDisk::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:CDP.ProtoDisk)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // double x = 1;
  if (!(this->x() <= 0 && this->x() >= 0)) {
    total_size += 1 + 8;
  }

  // double y = 2;
  if (!(this->y() <= 0 && this->y() >= 0)) {
    total_size += 1 + 8;
  }

  // double radius = 3;
  if (!(this->radius() <= 0 && this->radius() >= 0)) {
    total_size += 1 + 8;
  }

  // int32 size_type = 4;
  if (this->size_type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_size_type());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ProtoDisk::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:CDP.ProtoDisk)
  GOOGLE_DCHECK_NE(&from, this);
  const ProtoDisk* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ProtoDisk>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:CDP.ProtoDisk)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:CDP.ProtoDisk)
    MergeFrom(*source);
  }
}

void ProtoDisk::MergeFrom(const ProtoDisk& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:CDP.ProtoDisk)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (!(from.x() <= 0 && from.x() >= 0)) {
    _internal_set_x(from._internal_x());
  }
  if (!(from.y() <= 0 && from.y() >= 0)) {
    _internal_set_y(from._internal_y());
  }
  if (!(from.radius() <= 0 && from.radius() >= 0)) {
    _internal_set_radius(from._internal_radius());
  }
  if (from.size_type() != 0) {
    _internal_set_size_type(from._internal_size_type());
  }
}

void ProtoDisk::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:CDP.ProtoDisk)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ProtoDisk::CopyFrom(const ProtoDisk& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:CDP.ProtoDisk)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ProtoDisk::IsInitialized() const {
  return true;
}

void ProtoDisk::InternalSwap(ProtoDisk* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(ProtoDisk, size_type_)
      + sizeof(ProtoDisk::size_type_)
      - PROTOBUF_FIELD_OFFSET(ProtoDisk, x_)>(
          reinterpret_cast<char*>(&x_),
          reinterpret_cast<char*>(&other->x_));
}

::PROTOBUF_NAMESPACE_ID::Metadata ProtoDisk::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void ProtoPacking::InitAsDefaultInstance() {
}
class ProtoPacking::_Internal {
 public:
};

ProtoPacking::ProtoPacking(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena),
  disks_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:CDP.ProtoPacking)
}
ProtoPacking::ProtoPacking(const ProtoPacking& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      disks_(from.disks_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  dimensions_ = from.dimensions_;
  // @@protoc_insertion_point(copy_constructor:CDP.ProtoPacking)
}

void ProtoPacking::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_ProtoPacking_protos_2fcw_2eproto.base);
  dimensions_ = 0;
}

ProtoPacking::~ProtoPacking() {
  // @@protoc_insertion_point(destructor:CDP.ProtoPacking)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void ProtoPacking::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void ProtoPacking::ArenaDtor(void* object) {
  ProtoPacking* _this = reinterpret_cast< ProtoPacking* >(object);
  (void)_this;
}
void ProtoPacking::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void ProtoPacking::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ProtoPacking& ProtoPacking::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ProtoPacking_protos_2fcw_2eproto.base);
  return *internal_default_instance();
}


void ProtoPacking::Clear() {
// @@protoc_insertion_point(message_clear_start:CDP.ProtoPacking)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  disks_.Clear();
  dimensions_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* ProtoPacking::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 dimensions = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          dimensions_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated .CDP.ProtoDisk disks = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_disks(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<18>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ProtoPacking::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:CDP.ProtoPacking)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 dimensions = 1;
  if (this->dimensions() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_dimensions(), target);
  }

  // repeated .CDP.ProtoDisk disks = 2;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_disks_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(2, this->_internal_disks(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:CDP.ProtoPacking)
  return target;
}

size_t ProtoPacking::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:CDP.ProtoPacking)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .CDP.ProtoDisk disks = 2;
  total_size += 1UL * this->_internal_disks_size();
  for (const auto& msg : this->disks_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  // int32 dimensions = 1;
  if (this->dimensions() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_dimensions());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ProtoPacking::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:CDP.ProtoPacking)
  GOOGLE_DCHECK_NE(&from, this);
  const ProtoPacking* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ProtoPacking>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:CDP.ProtoPacking)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:CDP.ProtoPacking)
    MergeFrom(*source);
  }
}

void ProtoPacking::MergeFrom(const ProtoPacking& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:CDP.ProtoPacking)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  disks_.MergeFrom(from.disks_);
  if (from.dimensions() != 0) {
    _internal_set_dimensions(from._internal_dimensions());
  }
}

void ProtoPacking::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:CDP.ProtoPacking)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ProtoPacking::CopyFrom(const ProtoPacking& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:CDP.ProtoPacking)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ProtoPacking::IsInitialized() const {
  return true;
}

void ProtoPacking::InternalSwap(ProtoPacking* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  disks_.InternalSwap(&other->disks_);
  swap(dimensions_, other->dimensions_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ProtoPacking::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace CDP
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::CDP::ProtoDisk* Arena::CreateMaybeMessage< ::CDP::ProtoDisk >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CDP::ProtoDisk >(arena);
}
template<> PROTOBUF_NOINLINE ::CDP::ProtoPacking* Arena::CreateMaybeMessage< ::CDP::ProtoPacking >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CDP::ProtoPacking >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
