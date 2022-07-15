#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <ctime>
#include <string>

const int N = 50, START_NODE_ID = 1;
const std::unordered_set<int> PRIMES = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
const int PRINT_STEP = 1000'000'000;
int print_cnt = 0;
const std::string PATH_DELIM = ",";
const size_t MAX_PATH_LENGTH = std::min(8, N-1) + 2 * std::min(90, N-9) + PATH_DELIM.size() * N; // N < 100;

template <typename T, size_t N>
class Fixed_vector {
   size_t s;   
   public:
   T data[N];
 
   Fixed_vector() {s = 0;}
   T& operator[] (size_t n) {return data[n];}
   T* begin() {return data;}
   T* end() {return data+s;}   
   size_t size() const {return s;}
   size_t capacity() const {return N;}
   T front() {return data[0];}
   void push_back(T x) {data[s++] = x;}
   T pop_back() {return data[--s];}
 
   void erase(size_t pos) {
   	  for (size_t i = pos+1; i < s; ++i)
   		 data[i-1] = data[i];
   	  --s;
   }
 
   void clear() {s = 0;}
};

struct Node
{
	int id;
	Fixed_vector<int, N> links;
	bool visited;

	Node (int n = 0): id(n), visited(false) {};

	void print() {
		std::cout << id << ", [";
		for (auto link: links)
			std::cout << link << ", ";
		std::cout << "], " << (visited ? "visited" : "not visited\n");
	}
};

Fixed_vector<Node, N> nodes;
int node_indexes[N+1] = {};
Fixed_vector<int, N> longest_cycle, current_path;

void create_nodes (int n) {
 	nodes.clear();
 	const int HALF = n >> 1; 
 	for (int i = 1; i <= n; ++i) {
 		if (i > HALF && PRIMES.find(i) != PRIMES.end())
 			continue;

		auto new_node = Node(i);
		for (auto& node: nodes) {
			if (node.id % new_node.id && new_node.id % node.id)
				continue;
			new_node.links.push_back(node.id);
			node.links.push_back(new_node.id);
		} 

		node_indexes[i] = nodes.size();
		nodes.push_back(new_node);
 	}
}

void print_nodes() {
	for (auto& node: nodes)
		node.print();
}

Node& get_node(int node_id) {
	return nodes[node_indexes[node_id]];
}

int get_number_length(int n) {
	int res = 0;
	do {
		++res;
		n /= 10;
	} while (n);
	return res;
}

void _print_longest_cycle() {
	std::cout << "\r";
	size_t s = longest_cycle.size(), max_s = nodes.size();
	std::cout << s << "/" << max_s;
	size_t len = get_number_length(s) + 1 + get_number_length(max_s);
	for (size_t i = 0; i < (MAX_PATH_LENGTH - len); ++i)
		std::cout << " ";
	std::cout << std::endl;
	for (int id: longest_cycle)
		std::cout << id << PATH_DELIM;
	std::cout << std::endl;
}

void print_current_path() {
	std::cout << "\r";
	int len = 0;
	for (int id: current_path) {
		std::cout << id << PATH_DELIM;
		len += get_number_length(id) + PATH_DELIM.size();
	}
	std::cout << "???";
	len += 3;
	for (size_t i = 0; i < (MAX_PATH_LENGTH - len); ++i)
		std::cout << " ";
	std::cout << std::flush;
}

void update_longest_cycle() {
	size_t i;
	for (i = 0; i < longest_cycle.size(); ++i)
		longest_cycle[i] = current_path[i];

	for (; i < current_path.size(); ++i)
		longest_cycle.push_back(current_path[i]);

	_print_longest_cycle();
}

void process_node (int node_id) {
	auto& node = get_node(node_id);
	node.visited = true;
	current_path.push_back(node_id);

	if (print_cnt == PRINT_STEP)
		print_cnt = 0;
	if (print_cnt == 0)
		print_current_path();
	++print_cnt;

	for (auto link: node.links) {
		if (link == current_path.front()) {
			if (current_path.size() > longest_cycle.size())
				update_longest_cycle();
			continue;
		}

		const auto& link_node = get_node(link);
		if (link_node.visited)
			continue;

		process_node(link);
	}

	current_path.pop_back();
	node.visited = false;
}

void remove_most_massive_link(int node_id) {
	// В случае нашей задачи мы всегда удалим 2 из соседей вершины 1.
	auto& node = get_node(node_id);
	int max_neighb = 0;
	size_t max_link_index;
	for (size_t i = 0; i < node.links.size(); ++i) {
		const auto& link_node = get_node(node.links[i]);
		if (link_node.links.size() > max_neighb) {
			max_neighb = link_node.links.size();
			max_link_index = i;
		}
	}
	node.links.erase(max_link_index);
}

void print_longest_cycle() {
	longest_cycle.clear();
	current_path.clear();
	print_cnt = 0;

	// Одну из связей стартовой ноды можно не проверять, так как если с ней есть
	// цикл, то мы его проверим начиная с другой связи.
	remove_most_massive_link(START_NODE_ID);

	process_node(START_NODE_ID);
}

int main() {
	for (int n = 50; n <= N; ++n) {
		auto t0 = std::chrono::system_clock::now();
		std::time_t t0_time = std::chrono::system_clock::to_time_t(t0);

		std::cout << n << ": " << std::ctime(&t0_time);
		if (n < 4) {
			std::cout << n << "/" << nodes.size() << std::endl;
			std::cout << "1 2" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (PRIMES.find(n) != PRIMES.end()) {
			std::cout << "..." << std::endl;
			std::cout << std::endl;
			continue;
		}

		create_nodes(n);
		print_longest_cycle();

		auto t1 = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = t1 - t0;
		std::cout << elapsed_seconds.count() << "s" << std::endl;
		std::cout << std::endl;
	}

	return 0;
}

