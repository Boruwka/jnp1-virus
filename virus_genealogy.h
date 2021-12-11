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
#include <memory>

template<typename Virus>
class Vertex
{
public:
    std::shared_ptr<Virus> virus;
    Vertex(Virus::id_type const &stem_id)
    {
        virus = std::make_shared<Virus>(stem_id);
    }
private:
    std::unordered_set<std::shared_ptr<Vertex<Virus>>> children;
    std::unordered_set<std::shared_ptr<Vertex<Virus>>> parents;
};

template <typename Virus>
class VirusGenealogy
{
public:
    VirusGenealogy(Virus::id_type const &stem_id)
    {
        gen_stem_id = stem_id;
        graph.insert({stem_id, std::make_unique<Vertex<Virus>>(stem_id)});
    }

    Virus::id_type get_stem_id() const { return gen_stem_id; }

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




private:
    std::unordered_map<typename Virus::id_type, std::unique_ptr<Vertex<Virus>>> graph;
    Virus::id_type gen_stem_id;

};