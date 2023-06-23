/**
 * @file src/sequence_diagram/generators/json/sequence_diagram_generator.h
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
#pragma once

#include "common/generators/json/generator.h"
#include "config/config.h"
#include "sequence_diagram/model/diagram.h"
#include "util/util.h"

#include <glob/glob.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

namespace clanguml::sequence_diagram::generators::json {

std::string render_name(std::string name);

using diagram_config = clanguml::config::sequence_diagram;
using diagram_model = clanguml::sequence_diagram::model::diagram;

template <typename C, typename D>
using common_generator = clanguml::common::generators::json::generator<C, D>;

class generator : public common_generator<diagram_config, diagram_model> {
public:
    generator(diagram_config &config, diagram_model &model);

    void generate_call(const sequence_diagram::model::message &m,
        nlohmann::json &parent) const;

    common::id_t generate_participant(
        nlohmann::json &parent, common::id_t id, bool force = false) const;

    void generate_participant(
        nlohmann::json &parent, const std::string &name) const;

    void generate_activity(const sequence_diagram::model::activity &a,
        std::vector<common::model::diagram_element::id_t> &visited) const;

    void generate(std::ostream &ostr) const override;

    nlohmann::json &current_block_statement() const
    {
        assert(!block_statements_stack_.empty());

        return block_statements_stack_.back().get();
    }

private:
    bool is_participant_generated(common::id_t id) const;
    void process_call_message(const model::message &m,
        std::vector<common::model::diagram_element::id_t> &visited) const;
    void process_if_message(const model::message &m) const;
    void process_else_if_message() const;
    void process_end_if_message() const;
    void process_end_conditional_message() const;
    void process_conditional_else_message() const;
    void process_conditional_message(const model::message &m) const;
    void process_end_switch_message() const;
    void process_case_message(const model::message &m) const;
    void process_switch_message(const model::message &m) const;
    void process_end_try_message() const;
    void process_catch_message() const;
    void process_try_message(const model::message &m) const;
    void process_end_do_message() const;
    void process_do_message(const model::message &m) const;
    void process_end_for_message() const;
    void process_for_message(const model::message &m) const;
    void process_end_while_message() const;
    void process_while_message(const model::message &m) const;

    mutable std::set<common::id_t> generated_participants_;

    mutable nlohmann::json json_;

    mutable std::vector<std::reference_wrapper<nlohmann::json>>
        block_statements_stack_;
};

} // namespace clanguml::sequence_diagram::generators::json
