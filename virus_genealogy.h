#ifndef __VIRUS_GENEALOGY_H__
#define __VIRUS_GENEALOGY_H__

#include <iterator>
#include <memory>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>

struct VirusNotFound : public std::out_of_range {
    VirusNotFound() : std::out_of_range("VirusNotFound"){};
};

struct VirusAlreadyCreated : public std::exception {
    const char *what() const throw() { return "VirusAlreadyCreated"; }
};

struct TriedToRemoveStemVirus : public std::exception {
    const char *what() const throw() { return "TriedToRemoveStemVirus"; }
};

template <typename Virus> class VirusGenealogy {
public:
    VirusGenealogy(Virus::id_type const &stem_id);

    Virus::id_type get_stem_id() const { return stem_id; }

    VirusGenealogy(const VirusGenealogy &) = delete;

    VirusGenealogy &operator=(const VirusGenealogy &) = delete;

    class children_iterator;

    children_iterator get_children_begin(Virus::id_type const &id) const;

    children_iterator get_children_end(Virus::id_type const &id) const;

    bool exists(Virus::id_type const &id) const;

    const Virus &operator[](typename Virus::id_type const &id) const;

    std::vector<typename Virus::id_type> get_parents(Virus::id_type const &id) const;

    void create(Virus::id_type const &id, Virus::id_type const &parent_id);

    void create(Virus::id_type const &id,
                std::vector<typename Virus::id_type> const &parent_ids);

    void connect(Virus::id_type const &child_id, Virus::id_type const &parent_id);

    void remove(Virus::id_type const &id);

private:
    struct ClassCompare {
        bool operator() (const Virus::id_type& lhs, const Virus::id_type& rhs) const {
            return lhs < rhs;
        }
    };
    class Vertex;
    std::map<typename Virus::id_type, std::weak_ptr<Vertex>, ClassCompare> graph;
    std::shared_ptr<Vertex> stem;
    //usun id
    typename Virus::id_type stem_id;
};

//sprawdzic, czy sie da wstawic po prostu make shared
template <typename Virus>
VirusGenealogy<Virus>::VirusGenealogy(typename Virus::id_type const &stem_id) {
    stem = std::make_shared<Vertex>(stem_id);
    std::weak_ptr<Vertex> weak_stem = stem;
    graph.insert({stem_id, weak_stem});
    this->stem_id = stem_id;
}

template <typename Virus>
class VirusGenealogy<Virus>::Vertex {
private:
    std::shared_ptr<Virus> virus;

public:
    Vertex(Virus::id_type const &stem_id) {
        virus = std::make_shared<Virus>(stem_id);
    }

    std::shared_ptr<Virus> get_virus() const { return virus; }

    struct ClassCompareWeak {
        bool operator() (const std::weak_ptr<Vertex>& lhs, const std::weak_ptr<Vertex>& rhs) const {
            if (rhs.lock().get() == nullptr)
                return false;
            if (lhs.lock().get() == nullptr)
                return true;
            return lhs.lock()->get_virus()->get_id() < rhs.lock()->get_virus()->get_id();
        }
    };

    std::set<std::shared_ptr<Vertex>> children;
    std::set<std::weak_ptr<Vertex>, ClassCompareWeak> parents;
};

template <typename Virus>
class VirusGenealogy<Virus>::children_iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Virus;
    using pointer = std::shared_ptr<Virus>;
    using reference = const Virus &;

    children_iterator() = default;
    explicit children_iterator(typename std::set<std::shared_ptr<Vertex>>::iterator _it) {
        it = _it;
    }
    reference operator*() const { return *((*it)->get_virus()); }
    pointer operator->() { return (*it)->get_virus(); }
    children_iterator &operator++() {
        it++;
        return *this;
    }
    children_iterator operator++(int) {
        children_iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    children_iterator &operator--() {
        it--;
        return *this;
    }
    children_iterator operator--(int) {
        children_iterator tmp = *this;
        --(*this);
        return tmp;
    }
    friend bool operator==(const children_iterator &a, const children_iterator &b) {
        return a.it == b.it;
    };
    friend bool operator!=(const children_iterator &a, const children_iterator &b) {
        return a.it != b.it;
    };

private:
    typename std::set<std::shared_ptr<Vertex>>::iterator it;
};

template <typename Virus>
VirusGenealogy<Virus>::children_iterator
VirusGenealogy<Virus>::get_children_begin(typename Virus::id_type const &id) const {
    if (!exists(id)) {
        throw VirusNotFound();
    }
    return children_iterator(graph.at(id).lock()->children.begin());
}

template <typename Virus>
VirusGenealogy<Virus>::children_iterator
VirusGenealogy<Virus>::get_children_end(typename Virus::id_type const &id) const {
    if (!exists(id)) {
        throw VirusNotFound();
    }
    return children_iterator(graph.at(id).lock()->children.end());
}
template <typename Virus>
bool VirusGenealogy<Virus>::exists(typename Virus::id_type const &id) const {
    auto v_it = graph.find(id);
    if (v_it == graph.end()) {
        return false;
    }
    auto v = v_it->second;
    if (v.lock().get() == nullptr) {
        return false;
    }
    return true;
}

template <typename Virus>
const Virus &VirusGenealogy<Virus>::operator[](typename Virus::id_type const &id) const {
    if (!exists(id)) {
        throw VirusNotFound();
    }
    return *(graph.at(id).lock()->get_virus());
}

template <typename Virus>
std::vector<typename Virus::id_type>
VirusGenealogy<Virus>::get_parents(typename Virus::id_type const &id) const {
    if (!exists(id)) {
        throw VirusNotFound();
    }
    std::vector<typename Virus::id_type> parents_vector;
    for (auto parent : (graph.at(id).lock()->parents)) {
        if (parent.lock().get() != nullptr)
            parents_vector.push_back((*parent.lock()->get_virus()).get_id());
    }
    return parents_vector;
}

template <typename Virus>
void VirusGenealogy<Virus>::create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id) {
    if (exists(id)) {
        throw VirusAlreadyCreated();
    }
    if (!exists(parent_id)) {
        throw VirusNotFound();
    }
    std::shared_ptr<Vertex> v_new = std::make_shared<Vertex>(id);
    std::weak_ptr<Vertex> weak = v_new;
    graph.insert({id, weak});
    auto parent_vertex = graph.at(parent_id);
    v_new->parents.insert(parent_vertex);
    parent_vertex.lock()->children.insert(v_new);
}

template <typename Virus>
void VirusGenealogy<Virus>::create(typename Virus::id_type const &id,
                                   std::vector<typename Virus::id_type> const &parent_ids) {
    if (parent_ids.empty())
        return;

    if (exists(id)) {
        throw VirusAlreadyCreated();
    }
    for (auto parent_id : parent_ids) {
        if (!exists(parent_id)) {
            throw VirusNotFound();
        }
    }
    std::shared_ptr<Vertex> v_new = std::make_shared<Vertex>(id);
    std::weak_ptr<Vertex> weak = v_new;
    graph.insert({id, weak});
    for (auto parent_id : parent_ids) {
        auto parent_vertex = graph.at(parent_id);
        v_new->parents.insert(parent_vertex);
        parent_vertex.lock()->children.insert(v_new);
    }
}

template <typename Virus>
void VirusGenealogy<Virus>::connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id) {
    if (!exists(child_id)) {
        throw VirusNotFound();
    }
    if (!exists(parent_id)) {
        throw VirusNotFound();
    }
    auto child_vertex = graph.at(child_id);
    auto parent_vertex = graph.at(parent_id);
    child_vertex.lock()->parents.insert(parent_vertex);
    parent_vertex.lock()->children.insert(child_vertex.lock());
}

template <typename Virus>
void VirusGenealogy<Virus>::remove(typename Virus::id_type const &id) {
    if (id == stem_id) {
        throw TriedToRemoveStemVirus();
    }
    if (!exists(id)) {
        throw VirusNotFound();
    }
    auto v = graph.at(id).lock();
    for (auto parent : (v->parents)) {
        parent.lock()->children.erase(v);
    }
}

#endif