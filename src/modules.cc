// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#include "src/modules.h"

#include "src/ast-value-factory.h"

namespace v8 {
namespace internal {


void ModuleDescriptor::AddLocalExport(const AstRawString* export_name,
                                      const AstRawString* local_name,
                                      Zone* zone, bool* ok) {
  void* key = const_cast<AstRawString*>(export_name);

  ZoneAllocationPolicy allocator(zone);

  if (exports_ == nullptr) {
    exports_ = new (zone->New(sizeof(ZoneHashMap)))
        ZoneHashMap(ZoneHashMap::PointersMatch,
                    ZoneHashMap::kDefaultHashMapCapacity, allocator);
  }

  ZoneHashMap::Entry* p =
      exports_->Lookup(key, export_name->hash(), !IsFrozen(), allocator);
  if (p == nullptr || p->value != nullptr) {
    *ok = false;
  }

  p->value = const_cast<AstRawString*>(local_name);
}


void ModuleDescriptor::AddModuleRequest(const AstRawString* module_specifier,
                                        Zone* zone) {
  // TODO(adamk): Avoid this O(N) operation on each insert by storing
  // a HashMap, or by de-duping after parsing.
  if (requested_modules_.Contains(module_specifier)) return;
  requested_modules_.Add(module_specifier, zone);
}


const AstRawString* ModuleDescriptor::LookupLocalExport(
    const AstRawString* export_name, Zone* zone) {
  if (exports_ == nullptr) return nullptr;
  ZoneAllocationPolicy allocator(zone);
  ZoneHashMap::Entry* entry =
      exports_->Lookup(const_cast<AstRawString*>(export_name),
                       export_name->hash(), false, allocator);
  if (entry == nullptr) return nullptr;
  DCHECK_NOT_NULL(entry->value);
  return static_cast<const AstRawString*>(entry->value);
}
}
}  // namespace v8::internal
