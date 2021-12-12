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

        class children_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = Virus;
            using pointer           = std::unordered_set<std::shared_ptr<Vertex<Virus>>>::iterator;
            using reference         = Virus&;

            children_iterator() = default;
            explicit children_iterator(typename std::unordered_set<std::shared_ptr<Vertex<Virus>>>::iterator _it) {
                it = _it;
            }
            reference operator*() const { return *((*it)->virus);}
            pointer operator->() { return it; }
            children_iterator& operator++() { it++; return *this; }
            children_iterator operator++(int) { children_iterator tmp = *this; ++(*this); return tmp; }
            children_iterator& operator--() { it--; return *this; }
            children_iterator operator--(int) { children_iterator tmp = *this; --(*this); return tmp; }
            friend bool operator== (const children_iterator& a, const children_iterator& b) { return a.it == b.it; };
            friend bool operator!= (const children_iterator& a, const children_iterator& b) { return a.it != b.it; };
        private:
            typename std::unordered_set<std::shared_ptr<Vertex<Virus>>>::iterator it;
        };

        // bez wyjatkow
        VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
            return children_iterator(graph.at(id)->children.begin());
        }

        // bez wyjatkow
        VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
            return children_iterator(graph.at(id)->children.end());
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
                // tutaj rzucenie wyjątku Virus Not Found, ale jeszcze nwm jak
                // to miejsce daje warninga przy kompilacji, bo jeszcze nie ma wyjątku
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
                // tutaj też powinien być virus not found, to jest jeszcze do zrobienia
            }
        }

        void create(typename Virus::id_type const &id, Virus::id_type const &parent_id)
        {
           
            auto search = this->graph.find(id);
            if (search != this->graph.end())
            {
                // rzucenie virus already craeted
            }
            graph.insert({id, std::make_shared<Vertex<Virus>>(id)});
            connect(parent_id, id);
            // tu chyba jakieś try catch, ale to potem
        }

        void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids)
        {
            auto search = this->graph.find(id);
            if (search != this->graph.end())
            {
                // rzucenie virus already craeted
            }
            graph.insert({id, std::make_shared<Vertex<Virus>>(id)});
            for (auto parent_id: parent_ids)
            {
                connect(parent_id, id);
                // tu chyba jakieś try catch, ale to potem
            }
        }

        
        void remove(Virus::id_type const &id)
        {
            if (id == stem_id)
            {
                // rzucenie tried to remove stem virus
            }
            auto search = this->graph.find(id);
            if (search == this->graph.end())
            {
                // rzucenie virus not found
            }
            for (auto parent: (search->second->parents))
            {
                parent->children.erase(id);
            }
            graph.erase(id);
            
        }
        

    private:
        std::unordered_map<typename Virus::id_type, std::shared_ptr<Vertex<Virus>>> graph;
        typename Virus::id_type stem_id;
};
