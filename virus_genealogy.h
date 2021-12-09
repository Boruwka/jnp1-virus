#include <unordered_map>
#include <unordered_set>
#include <memory>

template<typename Virus>
class Vertex
{
    public:
        Vertex(Virus::id_type const &stem_id)
        {
            virus = std::make_unique<Virus>(stem_id);
        }
    private:
        std::unique_ptr<Virus> virus;
        std::unordered_set<typename Virus::id_type> children;
        std::unordered_set<typename Virus::id_type> parents;
};

template <typename Virus>
class VirusGenealogy
{
    public:
        VirusGenealogy(Virus::id_type const &stem_id)
        {
            graph.insert({stem_id, std::make_unique<Vertex<Virus>>(stem_id)});
        }
    private:
        std::unordered_map<typename Virus::id_type, std::unique_ptr<Vertex<Virus>>> graph;
};
