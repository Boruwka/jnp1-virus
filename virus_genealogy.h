/* Do zrobienia:
Karolina:
get_steam_id
get_children_begin
get_children_end
connect
wyjątki
Jagoda:
exists
operator[]
get_parents
create
remove
*/


#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <iterator>

struct VirusNotFound : public std::exception {
    const char* what() const throw() {
        return "VirusNotFound";
    }
};

struct VirusAlreadyCreated : public std::exception {
    const char* what() const throw() {
        return "VirusAlreadyCreated";
    }
};

struct TriedToRemoveStemVirus : public std::exception {
    const char* what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<typename Virus>
class Vertex
{
public:
    std::shared_ptr<Virus> virus;
    Vertex(Virus::id_type const &stem_id)
    {
        virus = std::make_shared<Virus>(stem_id);
    }
    std::unordered_set<std::shared_ptr<Vertex<Virus>>> children;
    std::unordered_set<std::shared_ptr<Vertex<Virus>>> parents;

    void insert_child(std::shared_ptr<Vertex<Virus>> child) {
        children.insert(child);
    }
    void insert_parent(std::shared_ptr<Vertex<Virus>> parent) {
        parents.insert(parent);
    }
};

template <typename Virus>
class VirusGenealogy
{
public:
    VirusGenealogy(Virus::id_type const &stem_id)
    {
        graph.insert({stem_id, std::make_shared<Vertex<Virus>>(stem_id)});
        this->stem_id = stem_id;
    }

    Virus::id_type get_stem_id() const { return stem_id; }

    VirusGenealogy(const VirusGenealogy&) = delete; // blokowanie konstruktora kopiującego

    VirusGenealogy& operator=(const VirusGenealogy&) = delete; //blokowanie operatora przypisania

    class children_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Virus;
        using pointer           = std::shared_ptr<Virus>;
        using reference         = Virus&;

        children_iterator() = default;
        explicit children_iterator(typename std::unordered_set<std::shared_ptr<Vertex<Virus>>>::iterator _it) {
            it = _it;
        }
        reference operator*() const { return *((*it)->virus);}
        pointer operator->() { return (*it)->virus; }
        children_iterator& operator++() { it++; return *this; }
        children_iterator operator++(int) { children_iterator tmp = *this; ++(*this); return tmp; }
        children_iterator& operator--() { it--; return *this; }
        children_iterator operator--(int) { children_iterator tmp = *this; --(*this); return tmp; }
        friend bool operator== (const children_iterator& a, const children_iterator& b) { return a.it == b.it; };
        friend bool operator!= (const children_iterator& a, const children_iterator& b) { return a.it != b.it; };
    private:
        typename std::unordered_set<std::shared_ptr<Vertex<Virus>>>::iterator it;
    };

    VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
        try {
            return children_iterator(graph.at(id)->children.begin());
        } catch (const std::out_of_range &e) {
            throw VirusNotFound();
        }
    }

    VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
        try {
            return children_iterator(graph.at(id)->children.end());
        } catch (const std::out_of_range &e) {
            throw VirusNotFound();
        }
    }

    bool exists(Virus::id_type const &id) const
    {
        return graph.contains(id);
    }

    const Virus& operator[](typename Virus::id_type const &id) const
    {
        auto search = this->graph.find(id);
        if (search != graph.end())
        {
            return *(search->second->virus);
        }
        else
        {
            throw VirusNotFound();
        }
    }


    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const
    {
        std::vector<typename Virus::id_type> parents_vector;
        auto search = this->graph.find(id);
        if (search != graph.end())
        {
            for (auto parent: (search->second->parents))
            {
                parents_vector.push_back((*parent->virus).get_id());
            }
            return parents_vector;

        }
        else
        {
            throw VirusNotFound();
        }
    }

    void create(typename Virus::id_type const &id, Virus::id_type const &parent_id)
    {

        auto search = this->graph.find(id);
        if (search != this->graph.end())
        {
            throw VirusAlreadyCreated();
        }
        graph.insert({id, std::make_shared<Vertex<Virus>>(id)});
        connect(id, parent_id); // czy tu trzeba robić try catch?
    }

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids)
    {
        auto search = this->graph.find(id);
        if (search != this->graph.end())
        {
            throw VirusAlreadyCreated();
        }
        graph.insert({id, std::make_shared<Vertex<Virus>>(id)});
        for (auto parent_id: parent_ids)
        {
            connect(id, parent_id);
            // czy tu trzeba robić try catch?
        }
    }

    void connect(Virus::id_type const &child_id, Virus::id_type const &parent_id) {
        try {
            auto child_vertex = graph.at(child_id);
            auto parent_vertex = graph.at(parent_id);
            child_vertex->insert_parent(parent_vertex);
            parent_vertex->insert_child(child_vertex);
        } catch (const std::out_of_range &e) {
            throw VirusNotFound();
        }

    }

    // void remove_single_vertex(std::unordered_map<typename Virus::id_type, std::shared_ptr<Vertex<Virus>>>::const_iterator search) i wtedy wszędzie zamiast vertex to search->second
    void remove_single_vertex(std::shared_ptr<Vertex<Virus>> vertex)
    {
        for (auto parent: (vertex->parents))
        {
            parent->children.erase(vertex);
        }
        for (auto child: (vertex->children))
        {
            child->parents.erase(vertex);
            if (child->parents.size() == 0)
            {
                remove_single_vertex(child);
            }
        }
        graph.erase(vertex->virus->get_id());
    }

    void remove(Virus::id_type const &id)
    {
        if (id == stem_id)
        {
            throw TriedToRemoveStemVirus();
        } 
        auto search = this->graph.find(id); 
        if (search == this->graph.end())
        {
            throw VirusNotFound();
        }
        remove_single_vertex(search->second);
    }


private:
    std::unordered_map<typename Virus::id_type, std::shared_ptr<Vertex<Virus>>> graph;
    typename Virus::id_type stem_id;
};
