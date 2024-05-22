#include "tid.h"

#include <algorithm> // std::find()
#include "reserved.h"

TID::TIDLine::TIDLine(std::string const & type_raw, std::string const & name_raw, std::string const & init_raw)
{
    set_type(type_raw);
    set_name(name_raw);
    set_init(init_raw);
}

void TID::TIDLine::set_type(std::string const & type_raw)
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

void TID::TIDLine::set_name(std::string const & name_raw)
{
    name_ = name_raw; // Без перекрытия имён
}

void TID::TIDLine::set_init(std::string const & init_raw)
{
    init_ = init_raw;
}

IDType TID::TIDLine::get_type() const
{
    return type_;
}

std::string TID::TIDLine::get_typename() const
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

std::string TID::TIDLine::get_name() const
{
    return name_;
}

std::string TID::TIDLine::get_init() const
{
    return init_;
}

bool TID::TIDLine::operator==(TIDLine const & other) const
{
    return name_ == other.name_;
}

bool TID::TIDLine::operator!=(TIDLine const & other) const
{
    return !(*this == other);
}

std::ostream & TID::TIDLine::print(std::ostream & os) const
{
    os << get_typename() << " " << name_ << " {" << init_ << "}";
    return os;
}

bool TID::add(std::string const & type, std::string const & name, std::string const & init)
{
    TIDLine new_line(type, name, init);
    if (std::find(lines_.begin(), lines_.end(), new_line) != lines_.end()) {
        return false;
    } else {
        lines_.push_back(new_line);
        return true;
    }
}

bool TID::set_init(std::string const & name, std::string const & init)
{
    TIDLine new_line("", name, "");
    auto it = std::find(lines_.begin(), lines_.end(), new_line);
    if (it != lines_.end()) {
        it->set_init(init);
        return true;
    } else {
        return false;
    }
}

std::ostream & operator<<(std::ostream & os, TID const & obj)
{
    for (auto const & line: obj.lines_) {
        line.print(os) << std::endl;
    }
    return os;
}
