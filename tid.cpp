#include "tid.h"

#include <iostream>
#include <algorithm>

TID::TID_Line::TID_Line(std::string const & type_raw, std::string const & name_raw, std::string const & init_raw)
{
    set_type(type_raw);
    set_name(name_raw);
    set_init(init_raw);
}

void TID::TID_Line::set_type(std::string const & type_raw)
{
    if (type_raw == "int") {
        type_ = ID_TYPE_INT;
    } else if (type_raw == "float") {
        type_ = ID_TYPE_FLOAT;
    } else if (type_raw == "bool") {
        type_ = ID_TYPE_BOOL;
    } else if (type_raw == "str") {
        type_ = ID_TYPE_STR;
    } else {
        type_ = ID_TYPE_UNKNOWN;
    }
}

void TID::TID_Line::set_name(std::string const & name_raw)
{
    name_ = name_raw; // Без перекрытия имён
}

void TID::TID_Line::set_init(std::string const & init_raw)
{
    init_ = init_raw;
}

id_types TID::TID_Line::get_type() const
{
    return type_;
}

std::string TID::TID_Line::get_name() const
{
    return name_;
}

std::string TID::TID_Line::get_init() const
{
    return init_;
}

bool TID::TID_Line::operator==(TID_Line const & other) const
{
    return name_ == other.name_;
}

bool TID::TID_Line::operator!=(TID_Line const & other) const
{
    return !(*this == other);
}

void TID::TID_Line::print() const
{
    std::cout << type_ << " " << name_ << " {" << init_ << "}" << std::endl;
}

bool TID::add(std::string const & type, std::string const & name, std::string const & init)
{
    TID_Line new_line(type, name, init);
    if (std::find(lines_.begin(), lines_.end(), new_line) != lines_.end()) {
        return false;
    } else {
        lines_.push_back(new_line);
        return true;
    }
}

bool TID::set_init(std::string const & name, std::string const & init)
{
    TID_Line new_line("", name, "");
    auto it = std::find(lines_.begin(), lines_.end(), new_line);
    if (it != lines_.end()) {
        it->set_init(init);
        return true;
    } else {
        return false;
    }
}

void TID::print() const
{
    for (auto const & elem: lines_) {
        elem.print();
    }
}
