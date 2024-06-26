#include "tid.h"

#include <algorithm> // std::find()
#include "reserved.h"

TID::ID::ID(std::string const & type_raw, std::string const & name_raw, std::string const & init_raw)
{
    set_type(type_raw);
    set_name(name_raw);
    set_init(init_raw);
}

void TID::ID::set_type(std::string const & type_raw)
{
    if (type_raw == rw::TYPE_INT) {
        type_ = ID_TYPE_INT;
    } else if (type_raw == rw::TYPE_FLOAT) {
        type_ = ID_TYPE_FLOAT;
    } else if (type_raw == rw::TYPE_BOOL) {
        type_ = ID_TYPE_BOOL;
    } else if (type_raw == rw::TYPE_STR) {
        type_ = ID_TYPE_STR;
    } else {
        type_ = ID_TYPE_UNKNOWN;
    }
}

void TID::ID::set_name(std::string const & name_raw)
{
    name_ = name_raw; // Без перекрытия имён
}

void TID::ID::set_init(std::string const & init_raw)
{
    init_ = init_raw;
}

IDType TID::ID::get_type() const
{
    return type_;
}

std::string TID::ID::get_typename() const
{
    if (type_ == ID_TYPE_INT) {
        return rw::TYPE_INT;
    } else if (type_ == ID_TYPE_FLOAT) {
        return rw::TYPE_FLOAT;
    } else if (type_ == ID_TYPE_BOOL) {
        return rw::TYPE_BOOL;
    } else if (type_ == ID_TYPE_STR) {
        return rw::TYPE_STR;
    } else {
        return "UNKNOWN_TYPE";
    }
}

std::string TID::ID::get_name() const
{
    return name_;
}

std::string TID::ID::get_init() const
{
    return init_;
}

bool TID::ID::operator==(ID const & other) const
{
    return name_ == other.name_;
}

bool TID::ID::operator!=(ID const & other) const
{
    return !(*this == other);
}

std::ostream & TID::ID::print(std::ostream & os) const
{
    os << get_typename() << " " << name_ << " {" << init_ << "}";
    return os;
}

int TID::find(std::string const & name) const
{
    ID tmp("", name, "");
    auto it = std::find(lines_.begin(), lines_.end(), tmp);
    if (it == lines_.end()) {
        return -1;
    } else {
        return it - lines_.begin();
    }
}

bool TID::add(std::string const & type, std::string const & name, std::string const & init)
{
    ID new_line(type, name, init);
    if (std::find(lines_.begin(), lines_.end(), new_line) == lines_.end()) {
        lines_.push_back(new_line);
        return true;
    } else {
        return false;
    }
}

bool TID::set_init(std::string const & name, std::string const & init)
{
    ID new_line("", name, "");
    auto it = std::find(lines_.begin(), lines_.end(), new_line);
    if (it == lines_.end()) {
        return false;
    } else {
        it->set_init(init);
        return true;
    }
}

std::ostream & operator<<(std::ostream & os, TID const & obj)
{
    for (auto const & line: obj.lines_) {
        line.print(os) << std::endl;
    }
    return os;
}
