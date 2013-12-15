#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "entity.hpp"

#include <string>
#include <tuple>
#include <vector>

class Level
{
    using Tile = int;
    using Row = std::vector<Tile>;
    using Field = std::vector<Row>;

    Field tiles;
    int width;
    int height;

    std::vector<std::shared_ptr<Entity>> entities;

  public:
    Level();
    Level(const std::string& filename);

    Tile& tileAt(int r, int c);

    int getWidth() const;
    int getHeight() const;

    void setWidth(int w);
    void setHeight(int h);

    Entity* newEntity();

    template <typename... Ts>
    std::vector<std::tuple<Entity*, Ts*...>> getEntities()
    {
        using Tuple = std::tuple<Entity*, Ts*...>;
        std::vector<Tuple> rval;

        rval.reserve(entities.size()/2+1);

        for (auto&& ptr : entities)
        {
            Tuple current;
            std::get<0>(current) = ptr.get();
            if (fill_tuple<1, Tuple, Ts...>(current)) rval.push_back(current);
        }

        return rval;
    }

  private:
    template <int N, typename T, typename U, typename... Vs>
    typename std::enable_if<
        N != std::tuple_size<T>::value-1
    , bool>::type fill_tuple(T& tup)
    {
        Entity*& e = std::get<0>(tup);
        std::get<N>(tup) = e->getComponent<U>();
        if (!std::get<N>(tup)) return false;
        return fill_tuple<N+1, T, Vs...>(tup);
    }

    template <int N, typename T, typename U>
    typename std::enable_if<
        N == std::tuple_size<T>::value-1
    , bool>::type fill_tuple(T& tup)
    {
        Entity*& e = std::get<0>(tup);
        std::get<N>(tup) = e->getComponent<U>();
        if (!std::get<N>(tup)) return false;
        return true;
    }

};

#endif // LEVEL_HPP