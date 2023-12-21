/**
 * tests/{{ name }}/test_case.h
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

TEST_CASE("{{ name }}", "[test-case][{{ type }}]")
{
    auto [config, db] = load_config("{{ name }}");

    auto diagram = config.diagrams["{{ name }}_{{ type }}"];

    REQUIRE(diagram->name == "{{ name }}_{{ type }}");

    auto model = generate_{{ type }}_diagram(*db, diagram);

    REQUIRE(model->name() == "{{ name }}_{{ type }}");

    {
        auto src = generate_{{ type }}_puml(diagram, *model);
        AliasMatcher _A(src);

        REQUIRE_THAT(src, StartsWith("@startuml"));
        REQUIRE_THAT(src, EndsWith("@enduml\n"));

        {{ examples }}

        save_puml(
            config.output_directory(), diagram->name + ".puml", src);
    }

    {
        auto j = generate_{{ type }}_json(diagram, *model);

        using namespace json;

        save_json(config.output_directory(), diagram->name + ".json", j);
    }

    {
        auto src = generate_{{ type }}_mermaid(diagram, *model);

        mermaid::AliasMatcher _A(src);
        using mermaid::IsClass;

        save_mermaid(config.output_directory(), diagram->name + ".mmd", src);
    }
}
