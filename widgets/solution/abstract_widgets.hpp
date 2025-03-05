#ifndef ABSTRACT_WIDGETS_HPP_
#define ABSTRACT_WIDGETS_HPP_

namespace widgets {

struct container;

struct widget {
    [[nodiscard]] virtual int width() const {
        return m_width;
    }

    [[nodiscard]] virtual int height() const {
        return m_height;
    }

    widget(const widget &) = delete;
    widget(widget &&) = delete;
    widget &operator=(const widget &) = delete;
    widget &operator=(widget &&) = delete;

    virtual ~widget() = default;

    [[nodiscard]] virtual widget *child_at(int x, int y);
    friend struct container;

    [[nodiscard]] container *parent() const {
        return m_parent;
    }

    void set_width(int width_) {
        m_width = width_;
    }

    void set_height(int height_) {
        m_height = height_;
    }

protected:
    widget() = default;

private:
    int m_width = 0;
    int m_height = 0;
    container *m_parent = nullptr;
};

struct container : widget {
    virtual void update_layout() = 0;

    static void set_parent(widget *parent, widget *child) {
        child->m_parent = dynamic_cast<container *>(parent);
    }
};
}  // namespace widgets

#endif  // ABSTRACT_WIDGETS_HPP_
