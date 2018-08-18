#ifndef CODE_GENERATOR_REPLACER_H
#define CODE_GENERATOR_REPLACER_H

#include <string>
#include <utility>
#include <vector>
#include <ostream>
#include <algorithm>
#include "util.h"

namespace code_generator
{
    class Replacer
    {
    public:
        Replacer() = delete;
        Replacer(const Replacer&) = default;
        Replacer& operator=(const Replacer&) = default;

        explicit Replacer(const std::string &input_file_name)
                : text{util::read_whole_file_as_string(input_file_name)}
        {}

        Replacer& add_tag(const std::string &tag, const std::string &value)
        {
            tags.emplace_back(tag, value);
            return *this;
        }

        // To be used after replace_tags()
        friend std::ostream& operator<<(std::ostream& os, const Replacer& printer)
        {
            os << printer.text;
            return os;
        }

        // To be used after all tags are added with add_tag()
        void replace_tags()
        {
            std::vector<TagPosition> tag_replacement_positions;
            std::string::size_type final_text_length = text.length();
            std::string::size_type greater_increment_text_length = text.length();
            for(auto& tag : tags)
            {
                const std::string full_tag{"[#" + tag.name + "]"};
                unsigned int count_pos = 0;
                auto tag_pos = text.find(full_tag);
                while (tag_pos != std::string::npos)
                {
                    tag_replacement_positions.emplace_back(tag,tag_pos);
                    tag_pos = text.find(full_tag,tag_pos+full_tag.length());
                    count_pos++;
                }
                greater_increment_text_length = std::max(greater_increment_text_length, text.length() - (full_tag.length() * count_pos) + (tag.value.length() * count_pos));
                final_text_length = final_text_length - (full_tag.length() * count_pos) + (tag.value.length() * count_pos);
            }
            std::string::size_type new_text_length = std::max(greater_increment_text_length,final_text_length);
            if(new_text_length > text.length())
            {
                text.reserve(new_text_length);
            }
            auto old_text_length = text.length();
            std::sort(tag_replacement_positions.begin(), tag_replacement_positions.end(),
                      [](const TagPosition& a, const TagPosition& b)
                      {
                          return a.position < b.position;
                      });
            for (auto tag_pos : tag_replacement_positions)
            {
                auto pos = tag_pos.position;
                pos += text.length() - old_text_length;
                text.replace(pos, tag_pos.tag.name.length() + 3, tag_pos.tag.value);
            }
        }
    private:
        struct Tag
        {
            Tag() = default;
            Tag(const Tag&) = default;
            Tag& operator=(const Tag&) = default;
            Tag(Tag&&) = default;
            Tag& operator=(Tag&&) = default;
            Tag(std::string name, std::string value)
                    : name(std::move(name)), value(std::move(value))
            {}

            std::string name;
            std::string value;
        };

        struct TagPosition
        {
            TagPosition(Tag tag, std::string::size_type position)
                    : tag(std::move(tag)), position(position)
            {}

            TagPosition() = default;
            TagPosition(const TagPosition&) = default;
            TagPosition& operator=(const TagPosition&) = default;
            TagPosition(TagPosition&&) = default;
            TagPosition& operator=(TagPosition&&) = default;


            Tag tag;
            std::string::size_type position{};
        };

        std::string text;
        std::vector<Tag> tags;
    };
}

#endif //CODE_GENERATOR_REPLACER_H
