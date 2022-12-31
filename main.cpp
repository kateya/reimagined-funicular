#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>

char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

size_t getN(unsigned int size) {
    size_t N = 1;
    unsigned long b = 256;
    for (;b <= size; N++, b*=256) {}
    return N;
}

void serialise(char *input, char *output)
{
    std::ifstream input_file(input, std::ios::in);
    std::ofstream output_file(output, std::ios::binary);
    unsigned int id1, id2, weight;

    std::unordered_map<unsigned int, std::vector<unsigned int>> map;
    std::unordered_map<unsigned int, unsigned int> ids;
    std::vector<unsigned int> ids_vector;

    while (input_file >> id1 >> id2 >> weight)
    {
        if (map.find(weight) != map.end()) {
            map[weight].push_back(id1);
            map[weight].push_back(id2);
        } else {
            map.emplace(weight, std::vector<unsigned int>{id1, id2});
        }
        if (ids.find(id1) == ids.end()) {
            ids.emplace(id1, ids.size());
            ids_vector.push_back(id1);
        }
        if (ids.find(id2) == ids.end()) {
            ids.emplace(id2, ids.size());
            ids_vector.push_back(id2);
        }
    }

    unsigned int len = ids_vector.size();
    output_file.write(reinterpret_cast<char *>(&len), sizeof(len));
    for (auto &elem : ids_vector) {
        output_file.write(reinterpret_cast<const char *>(&elem), sizeof(elem));
    }

    size_t N = getN(len);

    len = map.size();
    output_file.write(reinterpret_cast<char *>(&len), sizeof(len));

    for (std::pair<const unsigned int, std::vector<unsigned int> > &n : map)
    {
        output_file.write(reinterpret_cast<const char *>(&(n.first)), sizeof(char));
        unsigned int vector_len = n.second.size();
        output_file.write(reinterpret_cast<const char *>(&vector_len), N);
        for (auto &elem : n.second) {
            output_file.write(reinterpret_cast<char *>(&(ids[elem])), N);
        }
    }
    input_file.close();
    output_file.close();
}

void deserialise(char *input, char *output)
{
    std::ifstream input_file(input, std::ios::binary);
    std::ofstream output_file(output, std::ios::out);
    unsigned int id, id1, id2, weight;

    std::unordered_map<unsigned int, std::vector<unsigned int>> map;
    std::unordered_map<unsigned int, unsigned int> ids;
    unsigned int len;
    input_file.read(reinterpret_cast<char *>(&len), sizeof(len));

    for (int i = 0; i < len; i++)
    {
        input_file.read(reinterpret_cast<char *>(&id), sizeof(id));
        ids.emplace(i, id);
    }

    size_t N = getN(len);

    input_file.read(reinterpret_cast<char *>(&len), sizeof(len));
    for (int i = 0; i < len; i++)
    {
        input_file.read(reinterpret_cast<char *>(&weight), sizeof(char));
        unsigned int vector_len = 0;
        input_file.read(reinterpret_cast<char *>(&vector_len), N);
        for (int j = 0; j < vector_len; j += 2)
        {
            id1 = 0; id2 = 0;
            input_file.read(reinterpret_cast<char *>(&id1), N);
            input_file.read(reinterpret_cast<char *>(&id2), N);
            output_file << ids[id1] << "\t" << ids[id2] << "\t" << weight << std::endl;
        }
    }
    input_file.close();
    output_file.close();
}

int main(int argc, char *argv[])
{
    char *input = getCmdOption(argv, argv + argc, "-i");
    char *output = getCmdOption(argv, argv + argc, "-o");

    if (cmdOptionExists(argv, argv + argc, "-s"))
    {
        serialise(input, output);
    }
    else if (cmdOptionExists(argv, argv + argc, "-d"))
    {
        deserialise(input, output);
    }
    return 0;
}