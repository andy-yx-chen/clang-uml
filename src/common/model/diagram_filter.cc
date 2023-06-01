/**
 * src/common/model/diagram_filter.cc
 *
 * Copyright (c) 2021-2023 Bartek Kryza <bkryza@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diagram_filter.h"

#include <utility>

#include "class_diagram/model/class.h"
#include "common/model/package.h"
#include "include_diagram/model/diagram.h"
#include "package_diagram/model/diagram.h"

namespace clanguml::common::model {

namespace detail {

template <>
const clanguml::common::reference_vector<class_diagram::model::class_> &view(
    const class_diagram::model::diagram &d)
{
    return d.classes();
}

template <>
const clanguml::common::reference_vector<class_diagram::model::enum_> &view(
    const class_diagram::model::diagram &d)
{
    return d.enums();
}

template <>
const clanguml::common::reference_vector<common::model::package> &view(
    const package_diagram::model::diagram &d)
{
    return d.packages();
}

template <>
const clanguml::common::reference_vector<common::model::source_file> &view(
    const include_diagram::model::diagram &d)
{
    return d.files();
}

template <>
const clanguml::common::optional_ref<class_diagram::model::class_> get(
    const class_diagram::model::diagram &d, const std::string &full_name)
{
    return d.find<class_diagram::model::class_>(full_name);
}

template <>
const clanguml::common::optional_ref<common::model::package> get(
    const package_diagram::model::diagram &d, const std::string &full_name)
{
    return d.find<package>(full_name);
}

template <>
const clanguml::common::optional_ref<common::model::source_file> get(
    const include_diagram::model::diagram &d, const std::string &full_name)
{
    return d.find<source_file>(full_name);
}

template <>
clanguml::common::id_t destination_comparator<common::model::source_file>(
    const common::model::source_file &f)
{
    return f.id();
}
} // namespace detail

filter_visitor::filter_visitor(filter_t type)
    : type_{type}
{
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::element & /*e*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::relationship_t & /*r*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::access_t & /*a*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::namespace_ & /*ns*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::source_file & /*f*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram & /*d*/, const common::model::source_location & /*f*/) const
{
    return {};
}

tvl::value_t filter_visitor::match(
    const diagram &d, const class_diagram::model::class_method &m) const
{
    return match(d, m.access());
}

tvl::value_t filter_visitor::match(
    const diagram &d, const class_diagram::model::class_member &m) const
{
    return match(d, m.access());
}

bool filter_visitor::is_inclusive() const
{
    return type_ == filter_t::kInclusive;
}

bool filter_visitor::is_exclusive() const
{
    return type_ == filter_t::kExclusive;
}

filter_t filter_visitor::type() const { return type_; }

anyof_filter::anyof_filter(
    filter_t type, std::vector<std::unique_ptr<filter_visitor>> filters)
    : filter_visitor{type}
    , filters_{std::move(filters)}
{
}

tvl::value_t anyof_filter::match(
    const diagram &d, const common::model::element &e) const
{
    return tvl::any_of(filters_.begin(), filters_.end(),
        [&d, &e](const auto &f) { return f->match(d, e); });
}

tvl::value_t anyof_filter::match(
    const diagram &d, const common::model::source_file &e) const
{
    return tvl::any_of(filters_.begin(), filters_.end(),
        [&d, &e](const auto &f) { return f->match(d, e); });
}

namespace_filter::namespace_filter(
    filter_t type, std::vector<namespace_> namespaces)
    : filter_visitor{type}
    , namespaces_{std::move(namespaces)}
{
}

tvl::value_t namespace_filter::match(
    const diagram & /*d*/, const namespace_ &ns) const
{
    if (ns.is_empty())
        return {};

    return tvl::any_of(namespaces_.begin(), namespaces_.end(),
        [&ns](const auto &nsit) { return ns.starts_with(nsit) || ns == nsit; });
}

tvl::value_t namespace_filter::match(
    const diagram & /*d*/, const element &e) const
{
    if (dynamic_cast<const package *>(&e) != nullptr) {
        return tvl::any_of(namespaces_.begin(), namespaces_.end(),
            [&e, is_inclusive = is_inclusive()](const auto &nsit) {
                auto element_full_name_starts_with_namespace =
                    (e.get_namespace() | e.name()).starts_with(nsit);
                auto element_full_name_equals_pattern =
                    (e.get_namespace() | e.name()) == nsit;
                auto namespace_starts_with_element_qualified_name =
                    nsit.starts_with(e.get_namespace());

                auto result = element_full_name_starts_with_namespace ||
                    element_full_name_equals_pattern;

                if (is_inclusive)
                    result =
                        result || namespace_starts_with_element_qualified_name;

                return result;
            });
    }
    return tvl::any_of(namespaces_.begin(), namespaces_.end(),
        [&e](const auto &nsit) { return e.get_namespace().starts_with(nsit); });
}

element_filter::element_filter(filter_t type, std::vector<std::string> elements)
    : filter_visitor{type}
    , elements_{std::move(elements)}
{
}

tvl::value_t element_filter::match(
    const diagram & /*d*/, const element &e) const
{
    return tvl::any_of(
        elements_.begin(), elements_.end(), [&e](const auto &el) {
            return (e.full_name(false) == el) ||
                (fmt::format("::{}", e.full_name(false)) == el);
        });
}

element_type_filter::element_type_filter(
    filter_t type, std::vector<std::string> element_types)
    : filter_visitor{type}
    , element_types_{std::move(element_types)}
{
}

tvl::value_t element_type_filter::match(
    const diagram & /*d*/, const element &e) const
{
    return tvl::any_of(element_types_.begin(), element_types_.end(),
        [&e](const auto &element_type) {
            return e.type_name() == element_type;
        });
}

method_type_filter::method_type_filter(
    filter_t type, std::vector<config::method_type> method_types)
    : filter_visitor{type}
    , method_types_{std::move(method_types)}
{
}

tvl::value_t method_type_filter::match(
    const diagram &d, const class_diagram::model::class_method &m) const
{
    return tvl::any_of(
        method_types_.begin(), method_types_.end(), [&m](auto mt) {
            switch (mt) {
            case config::method_type::constructor:
                return m.is_constructor() || m.is_destructor();
            case config::method_type::assignment:
                return m.is_copy_assignment() || m.is_move_assignment();
            case config::method_type::operator_:
                return m.is_operator();
            case config::method_type::defaulted:
                return m.is_defaulted();
            case config::method_type::deleted:
                return m.is_deleted();
            case config::method_type::static_:
                return m.is_static();
            }

            return false;
        });
}

subclass_filter::subclass_filter(filter_t type, std::vector<std::string> roots)
    : filter_visitor{type}
    , roots_{std::move(roots)}
{
}

tvl::value_t subclass_filter::match(const diagram &d, const element &e) const
{
    if (d.type() != diagram_t::kClass)
        return {};

    if (roots_.empty())
        return {};

    if (!d.complete())
        return {};

    const auto &cd = dynamic_cast<const class_diagram::model::diagram &>(d);

    // First get all parents of element e
    clanguml::common::reference_set<class_diagram::model::class_> parents;

    const auto &fn = e.full_name(false);
    auto class_ref = cd.find<class_diagram::model::class_>(fn);

    if (!class_ref.has_value())
        return false;

    parents.emplace(class_ref.value());

    cd.get_parents(parents);

    std::vector<std::string> parents_names;
    for (const auto p : parents)
        parents_names.push_back(p.get().full_name(false));

    // Now check if any of the parents matches the roots specified in the
    // filter config
    for (const auto &root : roots_) {
        for (const auto &parent : parents) {
            auto full_name = parent.get().full_name(false);
            if (root == full_name)
                return true;
        }
    }

    return false;
}

parents_filter::parents_filter(filter_t type, std::vector<std::string> children)
    : filter_visitor{type}
    , children_{std::move(children)}
{
}

tvl::value_t parents_filter::match(const diagram &d, const element &e) const
{
    if (d.type() != diagram_t::kClass)
        return {};

    if (children_.empty())
        return {};

    if (!d.complete())
        return {};

    const auto &cd = dynamic_cast<const class_diagram::model::diagram &>(d);

    // First get all parents of element e
    clanguml::common::reference_set<class_diagram::model::class_> parents;

    for (const auto &child : children_) {
        auto child_ref = cd.find<class_diagram::model::class_>(child);
        if (!child_ref.has_value())
            continue;
        parents.emplace(child_ref.value());
    }

    cd.get_parents(parents);

    for (const auto &parent : parents) {
        if (e == parent.get())
            return true;
    }

    return false;
}

relationship_filter::relationship_filter(
    filter_t type, std::vector<relationship_t> relationships)
    : filter_visitor{type}
    , relationships_{std::move(relationships)}
{
}

tvl::value_t relationship_filter::match(
    const diagram & /*d*/, const relationship_t &r) const
{
    return tvl::any_of(relationships_.begin(), relationships_.end(),
        [&r](const auto &rel) { return r == rel; });
}

access_filter::access_filter(filter_t type, std::vector<access_t> access)
    : filter_visitor{type}
    , access_{std::move(access)}
{
}

tvl::value_t access_filter::match(
    const diagram & /*d*/, const access_t &a) const
{
    return tvl::any_of(access_.begin(), access_.end(),
        [&a](const auto &access) { return a == access; });
}

context_filter::context_filter(filter_t type, std::vector<std::string> context)
    : filter_visitor{type}
    , context_{std::move(context)}
{
}

tvl::value_t context_filter::match(const diagram &d, const element &e) const
{
    if (d.type() != diagram_t::kClass)
        return {};

    // Running this filter makes sense only after the entire diagram is
    // generated - i.e. during diagram generation
    if (!d.complete())
        return {};

    return tvl::any_of(context_.begin(), context_.end(),
        [&e, &d](const auto &context_root_name) {
            const auto &context_root =
                static_cast<const class_diagram::model::diagram &>(d)
                    .find<class_diagram::model::class_>(context_root_name);

            if (context_root.has_value()) {
                // This is a direct match to the context root
                if (context_root.value().id() == e.id())
                    return true;

                // Return a positive match if the element e is in a direct
                // relationship with any of the context_root's
                for (const relationship &rel :
                    context_root.value().relationships()) {
                    if (d.should_include(rel.type()) &&
                        rel.destination() == e.id())
                        return true;
                }
                for (const relationship &rel : e.relationships()) {
                    if (d.should_include(rel.type()) &&
                        rel.destination() == context_root.value().id())
                        return true;
                }

                // Return a positive match if the context_root is a parent
                // of the element
                for (const class_diagram::model::class_parent &p :
                    context_root.value().parents()) {
                    if (p.name() == e.full_name(false))
                        return true;
                }
                if (dynamic_cast<const class_diagram::model::class_ *>(&e) !=
                    nullptr) {
                    for (const class_diagram::model::class_parent &p :
                        static_cast<const class_diagram::model::class_ &>(e)
                            .parents()) {
                        if (p.name() == context_root.value().full_name(false))
                            return true;
                    }
                }
            }

            return false;
        });
}

paths_filter::paths_filter(filter_t type, const std::filesystem::path &root,
    const std::vector<std::filesystem::path> &p)
    : filter_visitor{type}
    , root_{root}
{
    for (const auto &path : p) {
        std::filesystem::path absolute_path;

        if (path.string().empty() || path.string() == ".")
            absolute_path = root;
        else if (path.is_relative())
            absolute_path = root / path;
        else
            absolute_path = path;

        try {
            absolute_path = absolute(absolute_path);
            absolute_path = canonical(absolute_path.lexically_normal());
        }
        catch (std::filesystem::filesystem_error &e) {
            LOG_WARN("Cannot add non-existent path {} to paths filter",
                absolute_path.string());
            continue;
        }

        absolute_path.make_preferred();

        paths_.emplace_back(std::move(absolute_path));
    }
}

tvl::value_t paths_filter::match(
    const diagram & /*d*/, const common::model::source_file &p) const
{
    if (paths_.empty()) {
        return {};
    }

    // Matching source paths doesn't make sens if they are not absolute
    if (!p.is_absolute()) {
        return {};
    }

    auto pp = p.fs_path(root_);
    for (const auto &path : paths_) {
        if (pp.root_name().string() == path.root_name().string() &&
            util::starts_with(pp.relative_path(), path.relative_path())) {

            return true;
        }
    }

    return false;
}

tvl::value_t paths_filter::match(
    const diagram & /*d*/, const common::model::source_location &p) const
{
    if (paths_.empty()) {
        return {};
    }

    auto sl_path = std::filesystem::path{p.file()};

    // Matching source paths doesn't make sens if they are not absolute or empty
    if (p.file().empty() || sl_path.is_relative()) {
        return {};
    }

    for (const auto &path : paths_) {
        if (sl_path.root_name().string() == path.root_name().string() &&
            util::starts_with(sl_path.relative_path(), path.relative_path())) {

            return true;
        }
    }

    return false;
}

class_method_filter::class_method_filter(filter_t type,
    std::unique_ptr<access_filter> af, std::unique_ptr<method_type_filter> mtf)
    : filter_visitor{type}
    , access_filter_{std::move(af)}
    , method_type_filter_{std::move(mtf)}
{
}

tvl::value_t class_method_filter::match(
    const diagram &d, const class_diagram::model::class_method &m) const
{
    tvl::value_t res = tvl::or_(
        access_filter_->match(d, m.access()), method_type_filter_->match(d, m));

    return res;
}

class_member_filter::class_member_filter(
    filter_t type, std::unique_ptr<access_filter> af)
    : filter_visitor{type}
    , access_filter_{std::move(af)}
{
}

tvl::value_t class_member_filter::match(
    const diagram &d, const class_diagram::model::class_member &m) const
{
    return access_filter_->match(d, m.access());
}

diagram_filter::diagram_filter(
    const common::model::diagram &d, const config::diagram &c)
    : diagram_{d}
{
    init_filters(c);
}

void diagram_filter::add_inclusive_filter(std::unique_ptr<filter_visitor> fv)
{
    inclusive_.emplace_back(std::move(fv));
}

void diagram_filter::add_exclusive_filter(std::unique_ptr<filter_visitor> fv)
{
    exclusive_.emplace_back(std::move(fv));
}

bool diagram_filter::should_include(
    const namespace_ &ns, const std::string &name) const
{
    if (should_include(ns)) {
        element e{namespace_{}};
        e.set_name(name);
        e.set_namespace(ns);

        return should_include(e);
    }

    return false;
}

void diagram_filter::init_filters(const config::diagram &c)
{
    // Process inclusive filters
    if (c.include) {
        add_inclusive_filter(std::make_unique<namespace_filter>(
            filter_t::kInclusive, c.include().namespaces));

        add_inclusive_filter(std::make_unique<relationship_filter>(
            filter_t::kInclusive, c.include().relationships));

        add_inclusive_filter(std::make_unique<access_filter>(
            filter_t::kInclusive, c.include().access));

        add_inclusive_filter(std::make_unique<paths_filter>(
            filter_t::kInclusive, c.root_directory(), c.include().paths));

        add_inclusive_filter(
            std::make_unique<class_method_filter>(filter_t::kInclusive,
                std::make_unique<access_filter>(
                    filter_t::kInclusive, c.include().access),
                std::make_unique<method_type_filter>(
                    filter_t::kInclusive, c.include().method_types)));

        add_inclusive_filter(
            std::make_unique<class_member_filter>(filter_t::kInclusive,
                std::make_unique<access_filter>(
                    filter_t::kInclusive, c.include().access)));

        // Include any of these matches even if one them does not match
        std::vector<std::unique_ptr<filter_visitor>> element_filters;

        element_filters.emplace_back(std::make_unique<element_filter>(
            filter_t::kInclusive, c.include().elements));

        element_filters.emplace_back(std::make_unique<element_type_filter>(
            filter_t::kInclusive, c.include().element_types));

        if (c.type() == diagram_t::kClass) {
            element_filters.emplace_back(std::make_unique<subclass_filter>(
                filter_t::kInclusive, c.include().subclasses));

            element_filters.emplace_back(std::make_unique<parents_filter>(
                filter_t::kInclusive, c.include().parents));

            element_filters.emplace_back(std::make_unique<
                edge_traversal_filter<class_diagram::model::diagram,
                    class_diagram::model::class_>>(filter_t::kInclusive,
                relationship_t::kInstantiation, c.include().specializations));

            element_filters.emplace_back(std::make_unique<
                edge_traversal_filter<class_diagram::model::diagram,
                    class_diagram::model::class_>>(filter_t::kInclusive,
                relationship_t::kDependency, c.include().dependants));

            element_filters.emplace_back(std::make_unique<
                edge_traversal_filter<class_diagram::model::diagram,
                    class_diagram::model::class_>>(filter_t::kInclusive,
                relationship_t::kDependency, c.include().dependencies, true));
        }
        else if (c.type() == diagram_t::kPackage) {
            element_filters.emplace_back(std::make_unique<edge_traversal_filter<
                    package_diagram::model::diagram, common::model::package>>(
                filter_t::kInclusive, relationship_t::kDependency,
                c.include().dependants));

            element_filters.emplace_back(std::make_unique<edge_traversal_filter<
                    package_diagram::model::diagram, common::model::package>>(
                filter_t::kInclusive, relationship_t::kDependency,
                c.include().dependencies, true));
        }
        else if (c.type() == diagram_t::kInclude) {
            std::vector<std::string> dependants;
            std::vector<std::string> dependencies;

            for (auto &&path : c.include().dependants) {
                const std::filesystem::path dep_path{path};
                dependants.emplace_back(dep_path.lexically_normal().string());
            }

            for (auto &&path : c.include().dependencies) {
                const std::filesystem::path dep_path{path};
                dependencies.emplace_back(dep_path.lexically_normal().string());
            }

            element_filters.emplace_back(std::make_unique<
                edge_traversal_filter<include_diagram::model::diagram,
                    common::model::source_file, common::model::source_file>>(
                filter_t::kInclusive, relationship_t::kAssociation,
                dependants));

            element_filters.emplace_back(std::make_unique<
                edge_traversal_filter<include_diagram::model::diagram,
                    common::model::source_file, common::model::source_file>>(
                filter_t::kInclusive, relationship_t::kAssociation,
                dependencies, true));
        }

        element_filters.emplace_back(std::make_unique<context_filter>(
            filter_t::kInclusive, c.include().context));

        add_inclusive_filter(std::make_unique<anyof_filter>(
            filter_t::kInclusive, std::move(element_filters)));
    }

    // Process exclusive filters
    if (c.exclude) {
        add_exclusive_filter(std::make_unique<namespace_filter>(
            filter_t::kExclusive, c.exclude().namespaces));

        add_exclusive_filter(std::make_unique<paths_filter>(
            filter_t::kExclusive, c.root_directory(), c.exclude().paths));

        add_exclusive_filter(std::make_unique<element_filter>(
            filter_t::kExclusive, c.exclude().elements));

        add_exclusive_filter(std::make_unique<element_type_filter>(
            filter_t::kExclusive, c.exclude().element_types));

        add_exclusive_filter(std::make_unique<relationship_filter>(
            filter_t::kExclusive, c.exclude().relationships));

        add_exclusive_filter(std::make_unique<access_filter>(
            filter_t::kExclusive, c.exclude().access));

        add_exclusive_filter(
            std::make_unique<class_method_filter>(filter_t::kExclusive,
                std::make_unique<access_filter>(
                    filter_t::kExclusive, c.exclude().access),
                std::make_unique<method_type_filter>(
                    filter_t::kExclusive, c.exclude().method_types)));

        add_exclusive_filter(
            std::make_unique<class_member_filter>(filter_t::kExclusive,
                std::make_unique<access_filter>(
                    filter_t::kExclusive, c.exclude().access)));

        add_exclusive_filter(std::make_unique<subclass_filter>(
            filter_t::kExclusive, c.exclude().subclasses));

        add_exclusive_filter(std::make_unique<parents_filter>(
            filter_t::kExclusive, c.exclude().parents));

        add_exclusive_filter(std::make_unique<edge_traversal_filter<
                class_diagram::model::diagram, class_diagram::model::class_>>(
            filter_t::kExclusive, relationship_t::kInstantiation,
            c.exclude().specializations));

        add_exclusive_filter(std::make_unique<edge_traversal_filter<
                class_diagram::model::diagram, class_diagram::model::class_>>(
            filter_t::kExclusive, relationship_t::kDependency,
            c.exclude().dependants));

        add_exclusive_filter(std::make_unique<edge_traversal_filter<
                package_diagram::model::diagram, common::model::package>>(
            filter_t::kExclusive, relationship_t::kDependency,
            c.exclude().dependants));

        add_exclusive_filter(std::make_unique<edge_traversal_filter<
                class_diagram::model::diagram, class_diagram::model::class_>>(
            filter_t::kExclusive, relationship_t::kDependency,
            c.exclude().dependencies, true));

        add_exclusive_filter(std::make_unique<edge_traversal_filter<
                package_diagram::model::diagram, common::model::package>>(
            filter_t::kExclusive, relationship_t::kDependency,
            c.exclude().dependencies, true));

        if (c.type() == diagram_t::kInclude) {
            std::vector<std::string> dependants;
            std::vector<std::string> dependencies;

            for (auto &&path : c.exclude().dependants) {
                std::filesystem::path dep_path{path};
                if (dep_path.is_relative()) {
                    dep_path = c.base_directory() / path;
                    dep_path = relative(dep_path, c.relative_to());
                }

                dependants.emplace_back(dep_path.lexically_normal().string());
            }

            for (auto &&path : c.exclude().dependencies) {
                std::filesystem::path dep_path{path};
                if (dep_path.is_relative()) {
                    dep_path = c.base_directory() / path;
                    dep_path = relative(dep_path, c.relative_to());
                }

                dependencies.emplace_back(dep_path.lexically_normal().string());
            }

            add_exclusive_filter(std::make_unique<
                edge_traversal_filter<include_diagram::model::diagram,
                    common::model::source_file>>(filter_t::kExclusive,
                relationship_t::kAssociation, dependencies, true));

            add_exclusive_filter(std::make_unique<
                edge_traversal_filter<include_diagram::model::diagram,
                    common::model::source_file>>(filter_t::kExclusive,
                relationship_t::kAssociation, dependants));
        }

        add_exclusive_filter(std::make_unique<context_filter>(
            filter_t::kExclusive, c.exclude().context));
    }
}

template <>
bool diagram_filter::should_include<std::string>(const std::string &name) const
{
    if (name.empty())
        return false;

    auto [ns, n] = common::split_ns(name);

    return should_include(ns, n);
}
} // namespace clanguml::common::model
