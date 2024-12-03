#include <vector>
#include <memory>

enum class Player {
    OPPONENT,
    SELF
};

class TemplateTile {
public:
    std::vector<std::shared_ptr<TemplateTile>> neighbours;

    TemplateTile(std::shared_ptr<TemplateTile> north_west_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> north_east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> south_east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> south_west_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> west_neighbour = nullptr);
};

class EmptyTile : public TemplateTile {
public:
    std::shared_ptr<Player> occupied_by;

    EmptyTile(std::shared_ptr<TemplateTile> north_west_neighbour = nullptr,
              std::shared_ptr<TemplateTile> north_east_neighbour = nullptr,
              std::shared_ptr<TemplateTile> east_neighbour = nullptr,
              std::shared_ptr<TemplateTile> south_east_neighbour = nullptr,
              std::shared_ptr<TemplateTile> south_west_neighbour = nullptr,
              std::shared_ptr<TemplateTile> west_neighbour = nullptr);
};

class OpponentTile : public TemplateTile {
public:
    Player occupied_by;

    OpponentTile(std::shared_ptr<TemplateTile> north_west_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> north_east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> south_east_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> south_west_neighbour = nullptr,
                 std::shared_ptr<TemplateTile> west_neighbour = nullptr);
};

class PlayerTile : public TemplateTile {
public:
    Player occupied_by;

    PlayerTile(std::shared_ptr<TemplateTile> north_west_neighbour = nullptr,
               std::shared_ptr<TemplateTile> north_east_neighbour = nullptr,
               std::shared_ptr<TemplateTile> east_neighbour = nullptr,
               std::shared_ptr<TemplateTile> south_east_neighbour = nullptr,
               std::shared_ptr<TemplateTile> south_west_neighbour = nullptr,
               std::shared_ptr<TemplateTile> west_neighbour = nullptr);
};

class Template {
public:
    int row;
    std::shared_ptr<TemplateTile> tile_of_origin;

    Template(int row, std::shared_ptr<TemplateTile> tile_of_origin);
};

class Templates {
public:
    static Template row_2;
    static std::vector<Template> row_3;
};
