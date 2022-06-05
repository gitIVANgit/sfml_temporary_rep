#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <stack>
#include <map>
#include <windows.h>



int count = 1;
sf::Vector2i pos1;
sf::Vector2i pos2;
//////////////////////
float currentImage = 0;
sf::Sprite sprite;
bool right;
bool up;
bool flag;
std::string map[4]{
    "31133133",
    "31131113",
    "33131133",
    "33333331"
};



struct Edge
{
    int weight;
    int indexTo;
};

struct Node
{
    int id;
    std::vector<Edge> edges;

    std::size_t prevIndex;
    int weight;
    bool visited;
};

std::vector<Node> nodes;


bool read_nodes(std::istream& istream, std::size_t nodes_count, std::vector<Node>& graph)
{
    nodes.clear();

    for (std::size_t i = 0; i < nodes_count; i++)
    {
        int id;
        istream >> id;
        nodes.push_back({ id });
    }

    return true;
}

bool read_edges(std::istream& istream, std::size_t edges_count, std::vector<Node>& graph)
{
    for (auto& node : nodes)
    {
        node.edges.clear();
    }

    for (std::size_t i = 0; i < edges_count; i++)
    {
        int start_id, end_id;
        int weight;

        istream >> start_id >> end_id;
        istream >> weight;

        std::vector<Node>& nodes_ref = nodes;

        auto start_iter = std::find_if(nodes_ref.begin(), nodes_ref.end(), [start_id](const auto& node) { return node.id == start_id; });
        auto end_iter = std::find_if(nodes_ref.begin(), nodes_ref.end(), [end_id](const auto& node) { return node.id == end_id; });

        if (start_iter == nodes_ref.end() || end_iter == nodes_ref.end())
        {
            for (auto node : nodes)
            {
                node.edges.clear();
            }
            std::cout << "some mistake!";
            return false;
        }
        (*start_iter).edges.push_back(Edge{ weight,end_id - 1 });
    }

    return true;
}

std::vector<Node> read_graph(const std::string& file_path)
{
    std::vector<Node> graph;
    std::ifstream fin(file_path);
    if (fin)
    {
        std::size_t node_count;
        int edge_count;

        fin >> node_count >> edge_count;

        if (read_nodes(fin, node_count, graph))
        {
            read_edges(fin, edge_count, graph);
        }
    }
    return graph;
}

std::vector<int> convert_graph_to_path(std::vector<Node>& graph, int start_index, int end_index)
{
    std::vector<int> result;
    std::stack<int> tmp_path;
    int current_node = end_index;
    while (current_node != 1000000)
    {
        tmp_path.push(current_node);
        current_node = nodes[current_node].prevIndex;
    }
    while (!tmp_path.empty())
    {
        result.push_back(tmp_path.top());
        tmp_path.pop();
    }
    return result;
}

std::vector<int> find_path_Dijkstra(std::vector<Node>& graph, int start_index, int end_index)
{
    for (auto& node : nodes)
    {
        node.weight = 1000000;
        node.visited = false;
        node.prevIndex = 1000000;
    }

    std::multimap<int, int> min_weigth_map;
    nodes[start_index].weight = 0;
    min_weigth_map.insert({ 0, start_index });

    while (!min_weigth_map.empty())
    {
        auto [current_weight, current_index] = *(min_weigth_map.begin());
        min_weigth_map.erase(min_weigth_map.begin());
        if (nodes[current_index].visited)
        {
            continue;
        }

        nodes[current_index].visited = true;

        for (int i = 0; i < nodes[current_index].edges.size(); i++)
        {
            int index_to = nodes[current_index].edges[i].indexTo;
            int edged_weight = nodes[current_index].edges[i].weight;
            if (!nodes[index_to].visited)
            {
                if (current_weight + edged_weight < nodes[index_to].weight)
                {
                    nodes[index_to].weight = current_weight + edged_weight;
                    nodes[index_to].prevIndex = current_index;
                    min_weigth_map.insert({ nodes[index_to].weight, index_to });
                }
            }
        }
         
    }

    return convert_graph_to_path(graph, start_index, end_index);
}




//
//void col(int a) {
//    for (int i = sprite.getPosition().y / 32; i < (sprite.getPosition().y + 32)/32; i++) {
//        for (int j = sprite.getPosition().x / 32; j < (sprite.getPosition().x + 32)/32; j++) {
//            if (map[i][j] == '1') {
//                if (right) {
//                    std::cout << "if"<<" "<<i << " "<<j<<"\n"; 
//                    sprite.setPosition(j * 32 - 32, sprite.getPosition().y);
//                }
//                else if(!right&&flag) {
//                    std::cout << "else" << " " << i << " " << j << "\n";
//                    sprite.setPosition(j * 32 + 32, sprite.getPosition().y);
//                }
//            }
//          
//        }
//        
//    }
//   
//}
//
//
//void col2(int a) {
//    for (int i = sprite.getPosition().y / 32; i < (sprite.getPosition().y + 32) / 32; i++) {
//        for (int j = sprite.getPosition().x / 32; j < (sprite.getPosition().x + 32) / 32; j++) {
//            if (map[i][j] == '1') {
//                if (up) {
//                    std::cout << "up if" << " " << i << " " << j << "\n";
//                    sprite.setPosition(sprite.getPosition().x, i * 32 + 32);
//                }
//                else if(!up&&!flag) {
//                    std::cout << "down else" << " " << i << " " << j << "\n";
//                    sprite.setPosition(sprite.getPosition().x, i * 32 - 32);
//                }
//
//            }
//
//        }
//    }
//}
//



int main()
{
  
    sf::RenderWindow window(sf::VideoMode(32*8, 32*4), "My window");
    window.setFramerateLimit(60);
    
    
    sf::Texture textureForSprite;
    textureForSprite.loadFromFile("water.png");

    sf::Sprite water(textureForSprite);



    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

           
        }
        

      
       if (count == 1 && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
           pos1 = sf::Mouse::getPosition();
           count++;
           std::cout << count<<"\n"<<(pos1.x-window.getPosition().x-10)/32 << " " << (pos1.y - window.getPosition().y-45) / 32 << "\n";
           Sleep(1000);
       }
       if (count == 2 && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
           pos2 = sf::Mouse::getPosition();
           count++;
           std::cout << count << "\n" << ((pos2.x - window.getPosition().x - 10) / 32) << " " << (pos2.y - window.getPosition().y - 45) / 32 << "\n";


           std::cout<< "\n\n"<<(pos1.x - window.getPosition().x - 10) / 32 + 8*((pos1.y - window.getPosition().y - 45) / 32) << "      " << ((pos2.x - window.getPosition().x - 10) / 32) + 8*((pos2.y - window.getPosition().y - 45) / 32) << "\n\n\n";
         
           
           
           std::vector<Node> graph = read_graph("input.txt");
           std::vector<int> path = find_path_Dijkstra(graph, (pos1.x - window.getPosition().x - 10) / 32 + 8 * ((pos1.y - window.getPosition().y - 45) / 32), (pos2.x - window.getPosition().x - 10) / 32 + 8 * ((pos2.y - window.getPosition().y - 45) / 32));
           std::map<int, int>coord;
           for (auto path_node_index : path)
           {

               if (int i = nodes[path_node_index].id - 1; i < 8) {
                   map[0][i] = '4';                   std::cout << nodes[path_node_index].id << " " << i << "\n";


                 }else if (int i = nodes[path_node_index].id - 1; i < 16) {
                     map[1][i - 8] = '4';                 std::cout << nodes[path_node_index].id << " " << i<<"\n";



                 }
                 else if (int i = nodes[path_node_index].id - 1; i < 24) {
                   map[2][i - 16] = '4';                                std::cout << nodes[path_node_index].id << " " << i << "\n";



               }
                 else if (int i = nodes[path_node_index].id - 1; i < 32) {
                   std::cout << nodes[path_node_index].id << " " << i << "\n";
                   map[3][i - 24] = '4';
               }
           }

           std::cout << "no mistake sleeping 3sec";
           Sleep(3000);
       }
       for (int i = 0; i < 4; i++) {
           for (int j = 0; j < 8; j++) {
               if (map[i][j] == '1') water.setTextureRect(sf::IntRect(1 * 32, 0, 32, 32));
               if (map[i][j] == '3') water.setTextureRect(sf::IntRect(2 * 32, 0, 32, 32));

               if (map[i][j] == '4') water.setTextureRect(sf::IntRect(0 * 32, 0, 32, 32));
               if (map[i][j] == ' ')continue;
               water.setPosition(j * 32, i * 32);
               window.draw(water);
           }
       }

       
        window.draw(sprite);
       window.display();
       window.clear(sf::Color::Black);

    }

    

}


