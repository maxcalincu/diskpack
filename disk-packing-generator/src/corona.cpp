#include <diskpack/corona.h>
#include <functional>
#include <iostream>
#include <numeric>

namespace diskpack {

/// Corona

SpiralSimilarityOperator
Corona::GetOperatorsProduct(const size_t &begin, const size_t &end,
                            const std::vector<SSORef> &operators) const {
  switch (end - begin) {
  case 0:
    return SpiralSimilarityOperator();
  case 1:
    return operators[begin];
  case 2:
    return operators[begin].get() * operators[begin + 1].get();
  case 3:
    return operators[begin].get() *
           (operators[begin + 1].get() * operators[begin + 2].get());
  case 4:
    return (operators[begin].get() * operators[begin + 1].get()) *
           (operators[begin + 2].get() * operators[begin + 3].get());
  }
  auto mid = (begin + end) / 2;
  return GetOperatorsProduct(begin, mid, operators) *
         GetOperatorsProduct(mid, end, operators);
}

bool Corona::IsCompleted() {
  assert(!corona.empty());
  auto cross_product =
      (corona.back()->get_center_x() - base.get_center_x()) *
          (corona.front()->get_center_y() - base.get_center_y()) -
      (corona.front()->get_center_x() - base.get_center_x()) *
          (corona.back()->get_center_y() - base.get_center_y());

  if (!(corona.size() > 2) || !corona.back()->tangent(*corona.front()) ||
      !cergt(cross_product, 0.0L)) {
    return false;
  }
  bool use_front = corona.front()->precision() < corona.back()->precision();
  Disk new_disk;
  const Disk &old_disk = *(!use_front ? corona.front() : corona.back());
  PeekNewDisk(new_disk, old_disk.get_type());
  bool check_intersect =
      !empty(intersect(new_disk.get_center_x(), old_disk.get_center_x())) &&
      !empty(intersect(new_disk.get_center_y(), old_disk.get_center_y()));
  return check_intersect;
}

void Corona::GetSortedCorona(const std::list<DiskPointer> &packing) {
  assert(corona.empty());
  for (auto &disk : packing) {
    if (base.tangent(*disk)) {
      corona.push_back(disk);
    }
  }
  std::sort(corona.begin(), corona.end(), DiskClockwiseCompare{base});
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::rotate(corona.begin(), corona.begin() + i + 1, corona.end());
      return;
    }
  }
}

bool Corona::IsContinuous() const {
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      return false;
    }
  }
  return true;
}

Corona::Corona(const Disk &b, const std::list<DiskPointer> &packing,
               OperatorLookupTable &lookup_table_)
    : base(b), lookup_table(lookup_table_) {
  operators_back.reserve(DEFAULT_OPERATORS_SIZE);
  operators_front.reserve(DEFAULT_OPERATORS_SIZE);

  GetSortedCorona(packing);
  assert(!corona.empty());
  leaf_front =
      IntervalPair{corona.front()->get_center_x() - base.get_center_x(),
                   corona.front()->get_center_y() - base.get_center_y()};

  leaf_back = IntervalPair{corona.back()->get_center_x() - base.get_center_x(),
                           corona.back()->get_center_y() - base.get_center_y()};
}

bool Corona::PeekNewDisk(Disk &new_disk, size_t index,
                         const std::optional<ConnectivityGraph> &graph) {
  bool use_front = corona.front()->precision() < corona.back()->precision();

  if (graph.has_value()) {
    if (!graph->HasTriangle(base.get_type(),
                            use_front ? corona.front()->get_type()
                                      : corona.back()->get_type(),
                            index)) {
      return false;
    }
  }

  auto &operators = (use_front) ? operators_front : operators_back;
  auto &starting_leaf = (use_front) ? leaf_front : leaf_back;

  operators.push_back(lookup_table(base.get_type(),
                                   use_front ? corona.front()->get_type()
                                             : corona.back()->get_type(),
                                   index));

  auto op = GetOperatorsProduct(0, operators.size(), operators);
  op.y *= (use_front ? -1 : 1);
  auto new_disk_center = op * starting_leaf;

  new_disk = Disk(new_disk_center.first + base.get_center_x(),
                  new_disk_center.second + base.get_center_y(),
                  lookup_table.radii[index], index);
  operators.pop_back();
  return true;
}

void Corona::Push(const DiskPointer &disk, size_t index) {
  bool use_front = corona.front()->precision() < corona.back()->precision();
  push_history.push(use_front);
  auto &operators = (use_front) ? operators_front : operators_back;
  operators.push_back(lookup_table(base.get_type(),
                                   use_front ? corona.front()->get_type()
                                             : corona.back()->get_type(),
                                   index));
  (use_front) ? corona.push_front(disk) : corona.push_back(disk);
}

void Corona::Pop() {
  auto use_front = push_history.top();
  push_history.pop();
  if (use_front) {
    operators_front.pop_back();
    corona.pop_front();
  } else {
    operators_back.pop_back();
    corona.pop_back();
  }
}

size_t ConnectivityGraph::Size() const {
  size_t result = 0;
  for (size_t base = 0; base < edges.size(); ++base) {
    result += signatures[base].size();
  }
  return result;
}

const Disk &Corona::GetBase() { return base; }

void Corona::DisplaySignature() {
  std::cerr << "signature: " << "\n";
  CoronaSignature signature(*this);
  std::cerr << "base: " << signature.base << "\n";
  for (size_t i = 0; i < lookup_table.radii.size(); ++i) {
    for (size_t j = 0; j < lookup_table.radii.size(); ++j) {
      std::cerr << signature.GetTransitions(i, j) << " ";
    }
    std::cerr << "\n";
  }
  for (auto &i : signature.specimen_indexes) {
    std::cerr << i << " ";
  }
  std::cerr << "\n";
}

/// CoronaSignature

size_t &CoronaSignature::GetTransitions(size_t i, size_t j) {
  return transitions[i > j ? (i * i + i) / 2 + j : (j * j + j) / 2 + i];
}
size_t CoronaSignature::GetTransitionsConst(size_t i, size_t j) const {
  return transitions[i > j ? (i * i + i) / 2 + j : (j * j + j) / 2 + i];
}
bool CoronaSignature::operator<(const CoronaSignature &other) const {
  for (size_t i = 0; i < transitions.size(); ++i) {
    if (transitions[i] != other.transitions[i]) {
      return transitions[i] < other.transitions[i];
    }
  }
  return false;
}
bool CoronaSignature::operator==(const CoronaSignature &other) const {
  for (size_t i = 0; i < transitions.size(); ++i) {
    if (transitions[i] != other.transitions[i]) {
      return false;
    }
  }
  return true;
}
CoronaSignature::CoronaSignature(Corona &specimen)
    : base(specimen.base.get_type()),
      radii_size(specimen.lookup_table.radii.size()),
      specimen_indexes(specimen.corona.size()),
      transitions((radii_size * radii_size + radii_size) / 2),
      disks(radii_size, false) {

  if (!specimen.IsCompleted()) {
    throw std::runtime_error("CoronaSignature takes in only completed coronas");
  }
  for (size_t i = 0; i < specimen_indexes.size(); ++i) {
    specimen_indexes[i] = specimen.corona[i]->get_type();
    disks[specimen_indexes[i]] = true;
  }
  for (size_t i = 0; i + 1 < specimen_indexes.size(); ++i) {
    ++GetTransitions(specimen_indexes[i], specimen_indexes[i + 1]);
  }
  ++GetTransitions(specimen_indexes[specimen_indexes.size() - 1],
                   specimen_indexes[0]);
}

bool CoronaSignature::TestRadii(OperatorLookupTable &lookup_table) const {
  auto &new_radii = lookup_table.radii;
  Disk b(0, 0, new_radii[base], base);
  std::list<DiskPointer> packing;
  packing.push_back(std::make_shared<Disk>(
      std::move(Disk(new_radii[base] + new_radii[specimen_indexes[0]], 0,
                     new_radii[specimen_indexes[0]], specimen_indexes[0]))));
  Corona test(b, packing, lookup_table);
  size_t cur_front = 0, cur_back = 0;
  for (size_t i = 1; i < specimen_indexes.size(); ++i) {
    auto use_front =
        test.corona.front()->precision() < test.corona.back()->precision();
    if (use_front) {
      cur_front =
          (cur_front == 0 ? specimen_indexes.size() - 1 : cur_front - 1);
    } else {
      cur_back = (cur_back == specimen_indexes.size() - 1 ? 0 : cur_back + 1);
    }
    size_t next = specimen_indexes[(use_front ? cur_front : cur_back)];
    Disk new_disk;
    test.PeekNewDisk(new_disk, next);
    if (std::any_of(packing.begin(), packing.end(),
                    [&new_disk](const DiskPointer &disk) {
                      return new_disk.intersects(*disk);
                    })) {
      return false;
    }
    packing.push_back(std::make_shared<Disk>(std::move(new_disk)));
    test.Push(packing.back(), next);
  }
  return test.IsCompleted();
}

size_t CoronaSignature::GetBase() const { return base; }

bool CoronaSignaturePointerCompare(const CoronaSignaturePointer &a,
                                   const CoronaSignaturePointer &b) {
  return *a < *b;
}

/// ConnectivityGraph

void ConnectivityGraph::CoronaFill(
    Corona &corona, std::list<DiskPointer> &packing, size_t starting_index,
    std::set<CoronaSignaturePointer, decltype(&CoronaSignaturePointerCompare)>
        &unique_signatures) {
  auto &radii = corona.lookup_table.radii;
  if (corona.IsCompleted()) {
    CoronaSignaturePointer signature =
        std::make_shared<CoronaSignature>(corona);
    if (unique_signatures.insert(signature).second) {
      signatures[corona.base.get_type()].push_back(signature);
      Push(*signature);
    }
    return;
  }

  Disk new_disk;

  for (size_t i = starting_index; i < radii.size(); ++i) {
    corona.PeekNewDisk(new_disk, i);

    if (std::any_of(packing.begin(), packing.end(),
                    [&new_disk](const DiskPointer &disk) {
                      return new_disk.intersects(*disk);
                    })) {
      continue;
    }
    if (new_disk.precision() > PRECISION_THRESHOLD) {
      is_broken = true;
      return;
    }

    packing.push_back(std::make_shared<Disk>(std::move(new_disk)));
    corona.Push(packing.back(), i);

    CoronaFill(corona, packing, starting_index, unique_signatures);

    corona.Pop();
    packing.pop_back();
    if (HasOverflow()) {
      return;
    }
  }
}

ConnectivityGraph::ConnectivityGraph(OperatorLookupTable &lookup_table)
    : diffs(lookup_table.radii.size()), signatures(lookup_table.radii.size()),
      transitions(lookup_table.radii.size(),
                  std::vector<size_t>(
                      (lookup_table.radii.size() * lookup_table.radii.size() +
                       lookup_table.radii.size()) /
                          2,
                      0)),
      edges(lookup_table.radii.size(),
            std::vector<bool>(lookup_table.radii.size(), false)) {
  std::list<DiskPointer> packing;
  auto &radii = lookup_table.radii;
  PRECISION_THRESHOLD =
      std::min_element(radii.begin(), radii.end(),
                       [](const Interval &x, const Interval &y) {
                         return x.lower() < y.lower();
                       })
          ->lower() /
      3.0L;

  for (size_t base = 0; base < radii.size(); ++base) {
    Disk base_disk(zero, zero, radii[base], base);
    std::set<CoronaSignaturePointer, decltype(&CoronaSignaturePointerCompare)>
        unique_signatures(CoronaSignaturePointerCompare);
    for (size_t starting_index = 0; starting_index < radii.size();
         ++starting_index) {
      packing.push_back(
          std::make_shared<Disk>(radii[base] + radii[starting_index], zero,
                                 radii[starting_index], starting_index));
      Corona corona(base_disk, packing, lookup_table);
      CoronaFill(corona, packing, starting_index, unique_signatures);
      packing.clear();
    }
  }
  for (size_t base = 0; base < edges.size(); ++base) {
    for (size_t i = 0; i < edges.size(); ++i) {
      for (size_t j = i; j < edges.size(); ++j) {
        if (GetTransitionsConst(base, i, j) == 0) {
          redundant_triangles.push(std::make_tuple(base, i, j));
        }
      }
    }
  }
  if (HasOverflow()) {
    return;
  }
  RemoveRedundantTriangles();
  UpdateEdges();
}

void ConnectivityGraph::DisplaySignatures() const {
  for (size_t base = 0; base < edges.size(); ++base) {
    for (auto signature : signatures[base]) {
      std::cerr << base << " : ";
      for (auto index : signature->specimen_indexes) {
        std::cerr << index << " ";
      }
      std::cerr << "\n";
    }
  }
}

void ConnectivityGraph::Refine(OperatorLookupTable &lookup_table) {
  auto &radii = lookup_table.radii;

  std::shared_ptr<std::vector<std::shared_ptr<SignatureList>>>
      invalid_signatures =
          std::make_shared<std::vector<std::shared_ptr<SignatureList>>>(
              diffs.size());
  for (auto &list : *invalid_signatures) {
    list = std::make_shared<SignatureList>();
  }

  for (size_t base = 0; base < radii.size(); ++base) {
    for (auto it = signatures[base].begin(); it != signatures[base].end();) {
      auto signature = *it;
      if (signature->TestRadii(lookup_table)) {
        ++it;
        continue;
      }
      invalid_signatures->operator[](base)->push_back(signature);
      it = signatures[base].erase(it);
      Pop(*signature);
    }
  }
  RemoveRedundantTriangles(invalid_signatures);
  for (size_t base = 0; base < edges.size(); ++base) {
    diffs[base].push(invalid_signatures->operator[](base));
  }
  UpdateEdges();
}

bool ConnectivityGraph::Restore() {
  if (std::any_of(diffs.begin(), diffs.end(),
                  [](const VersionDiffStack &s) { return s.empty(); })) {
    return false;
  }
  for (size_t base = 0; base < diffs.size(); ++base) {
    auto diff = diffs[base].top();
    diffs[base].pop();
    for (auto signature : *diff) {
      Push(*signature);
    }
    signatures[base].splice(signatures[base].end(), std::move(*diff));
  }
  UpdateEdges();
  return true;
}

bool ConnectivityGraph::HasOverflow() const {
  return is_broken || std::any_of(signatures.begin(), signatures.end(),
                                  [this](const SignatureList &x) {
                                    return x.size() > MAX_CORONA_SIGNATURES;
                                  });
}

void ConnectivityGraph::Push(const CoronaSignature &signature) {
  for (size_t x = 0; x < signature.transitions.size(); ++x) {
    transitions[signature.base][x] += signature.transitions[x];
  }
}

void ConnectivityGraph::Pop(const CoronaSignature &signature) {
  for (size_t i = 0; i < edges.size(); ++i) {
    for (size_t j = i; j < edges.size(); ++j) {
      GetTransitions(signature.base, i, j) -=
          signature.GetTransitionsConst(i, j);
      if (signature.GetTransitionsConst(i, j) > 0 &&
          GetTransitionsConst(signature.base, i, j) == 0) {
        redundant_triangles.push(std::make_tuple(i, j, signature.base));
      }
    }
  }
}

void ConnectivityGraph::RemoveRedundantTriangles(
    std::shared_ptr<std::vector<std::shared_ptr<SignatureList>>> diff) {
  std::vector<SignatureList> removed_signatures(edges.size());
  while (!redundant_triangles.empty()) {
    auto [i, j, k] = redundant_triangles.front();
    redundant_triangles.pop();
    std::vector<size_t> ijk{i, j, k};
    for (size_t rotation_index = 0; rotation_index < ijk.size();
         ++rotation_index) {
      std::rotate(ijk.begin(), std::next(ijk.begin()), ijk.end());
      if (GetTransitionsConst(ijk[0], ijk[1], ijk[2]) == 0) {
        continue;
      }
      for (auto it = signatures[ijk[0]].begin();
           it != signatures[ijk[0]].end();) {
        auto signature = *it;
        if (signature->GetTransitionsConst(ijk[1], ijk[2]) > 0) {
          removed_signatures[ijk[0]].push_back(signature);
          it = signatures[ijk[0]].erase(it);
          Pop(*signature);
        } else {
          ++it;
        }
      }
    }
  }

  if (diff) {
    for (size_t base = 0; base < edges.size(); ++base) {
      diff->operator[](base)->splice(diff->operator[](base)->end(),
                                     std::move(removed_signatures[base]));
    }
  }
}

size_t &ConnectivityGraph::GetTransitions(size_t base, size_t i, size_t j) {
  return transitions[base][i > j ? (i * i + i) / 2 + j : (j * j + j) / 2 + i];
}

size_t ConnectivityGraph::GetTransitionsConst(size_t base, size_t i,
                                              size_t j) const {
  return transitions[base][i > j ? (i * i + i) / 2 + j : (j * j + j) / 2 + i];
}

void ConnectivityGraph::UpdateEdges() {
  for (size_t i = 0; i < edges.size(); ++i) {
    for (size_t j = i; j < edges.size(); ++j) {
      edges[i][j] = false;
      for (size_t x = 0; x < edges.size(); ++x) {
        edges[i][j] = edges[i][j] || (GetTransitionsConst(i, j, x) > 0 &&
                                      GetTransitionsConst(j, i, x) > 0);
        edges[j][i] = edges[i][j];
      }
    }
  }
}

bool ConnectivityGraph::IsViable() const {
  // graph G with one vertex for each disk size, and an edge between i and j (j
  // can be equal to i, i.e. they may have loop) if there is a disk x s.t.
  //  * there is an i-corona with an angle jx (that is, two consecutive disks of
  //  radius j and x)
  //  * there is a j-corona with an angle ix
  //  (make the picture: it means the coronas allow three mutually adjacent
  //  disks i,j,x, which is clearly necessary to have disks i and j in contact)

  /// Condition 1: Graph G is connected

  // DisplaySignatures();
  {
    std::vector<bool> unvisited(edges.size(), true);
    std::queue<size_t> q;
    q.push(0);
    unvisited[0] = false;
    while (!q.empty()) {
      auto vertice = q.front();
      q.pop();
      for (size_t neighbor = 0; neighbor < edges.size(); ++neighbor) {
        if (unvisited[neighbor] && edges[vertice][neighbor]) {
          unvisited[neighbor] = false;
          q.push(neighbor);
        }
      }
    }
    if (std::any_of(unvisited.begin(), unvisited.end(),
                    [](bool b) { return b; })) {
      return false;
    }
  }

  /// Condition 2: if removing a vertex i disconnects G into G1,...Gk,
  ///              then there must be an i-corona with at least one disk in each
  ///              of G1,...,Gk
  {
    for (size_t disconnected_vertice = 0; disconnected_vertice < edges.size();
         ++disconnected_vertice) {
      std::vector<size_t> components(edges.size(), 0);
      components[disconnected_vertice] = edges.size();
      size_t cur_component = 0;
      for (size_t vertice = 0; vertice < edges.size(); ++vertice) {
        if (components[vertice]) {
          continue;
        }
        ++cur_component;
        std::queue<size_t> q;
        q.push(vertice);
        components[vertice] = cur_component;
        while (!q.empty()) {
          size_t x = q.front();
          q.pop();
          for (size_t neighbor = 0; neighbor < edges.size(); ++neighbor) {
            if (components[neighbor] || !edges[x][neighbor]) {
              continue;
            }
            components[neighbor] = cur_component;
            q.push(neighbor);
          }
        }
      }

      if (!std::any_of(signatures[disconnected_vertice].begin(),
                       signatures[disconnected_vertice].end(),
                       [cur_component, this,
                        &components](const CoronaSignaturePointer &signature) {
                         for (size_t component = 1; component <= cur_component;
                              ++component) {
                           bool connects_component = false;
                           for (size_t vertice = 0; vertice < edges.size();
                                ++vertice) {
                             connects_component =
                                 connects_component ||
                                 (signature->disks[vertice] &&
                                  components[vertice] == component);
                           }
                           if (!connects_component) {
                             return false;
                           }
                         }
                         return true;
                       })) {
        return false;
      }
    }
  }

  // Condition 3: the graph H whose vertices are the coronas with an edge
  // between two coronas if they are "compatible"
  //              (which means there is a triangle i,j,x for some x in the
  //              previous graph G). This graph H must contains a connected
  //              component with all types of coronas (i. e. with all types of
  //              corona bases).
  {
    std::vector<size_t> components(edges.size() * edges.size() * edges.size());
    std::vector<size_t> component_size(components.size(), 1);
    std::iota(components.begin(), components.end(), 0);

    auto get_index = [&](size_t i, size_t j, size_t k) {
      std::vector<size_t> ijk{i, j, k};
      std::sort(ijk.begin(), ijk.end());
      return ijk[0] + edges.size() * ijk[1] +
             (edges.size() * edges.size()) * ijk[2];
    };
    std::function<size_t(size_t)> get_parent = [&](size_t x) {
      components[x] = (components[x] == x ? x : get_parent(components[x]));
      return components[x];
    };
    std::function<void(size_t, size_t size_t)> unite = [&](size_t x, size_t y) {
      x = get_parent(x);
      y = get_parent(y);
      if (x != y) {
        if (component_size[x] > component_size[y]) {
          std::swap(x, y);
        }
        components[x] = y;
        component_size[y] += component_size[x];
      }
    };

    for (size_t base = 0; base < edges.size(); ++base) {
      for (auto signature : signatures[base]) {
        size_t first_i = signature->specimen_indexes[0],
               first_j = signature->specimen_indexes[1];
        size_t first_index = get_index(base, first_i, first_j);

        for (size_t i = 0; i < edges.size(); ++i) {
          for (size_t j = i; j < edges.size(); ++j) {
            if (signature->GetTransitionsConst(i, j)) {
              unite(first_index, get_index(base, i, j));
            }
          }
        }
      }
    }

    std::vector<std::vector<bool>> component_disk_types(
        edges.size() * edges.size() * edges.size(),
        std::vector<bool>(edges.size(), false));
    for (size_t i = 0; i < edges.size(); ++i) {
      for (size_t j = i; j < edges.size(); ++j) {
        for (size_t k = j; k < edges.size(); ++k) {
          size_t component = get_parent(get_index(i, j, k));

          component_disk_types[component][i] = true;
          component_disk_types[component][j] = true;
          component_disk_types[component][k] = true;
        }
      }
    }

    if (!std::any_of(component_disk_types.begin(), component_disk_types.end(),
                     [](const std::vector<bool> &disks) {
                       return !std::any_of(disks.begin(), disks.end(),
                                           [](bool u) { return !u; });
                     })) {
      return false;
    }
  }

  return true;
}

bool ConnectivityGraph::HasTriangle(size_t i, size_t j, size_t k) const {
  return GetTransitionsConst(i, j, k) && GetTransitionsConst(k, i, j) &&
         GetTransitionsConst(j, k, i);
}

} // namespace diskpack