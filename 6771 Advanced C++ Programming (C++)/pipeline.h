#ifndef COMP6771_PIPELINE_H
#define COMP6771_PIPELINE_H

#include <type_traits>
#include <unordered_map>
#include <concepts>
#include <cstddef>
#include <exception>
#include <optional>
#include <string>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <utility>
#include <vector>
#include <queue>
#include <list>

namespace ppl {

    // Errors that may occur in a pipeline.
    enum class pipeline_error_kind {
        // An expired node ID was provided.
        invalid_node_id,
        // Attempting to bind a non-existant slot.
        no_such_slot,
        // Attempting to bind to a slot that is already filled.
        slot_already_used,
        // The output type and input types for a connection don't match.
        connection_type_mismatch,
    };
    struct pipeline_error : public std::exception {
        explicit pipeline_error(pipeline_error_kind kind) : kind_{kind}{}
        auto kind() const noexcept -> pipeline_error_kind {return kind_;}
        auto what() const noexcept -> const char * override {
            switch (kind()) {
                case pipeline_error_kind::invalid_node_id:
                    return "invalid node ID";
                case pipeline_error_kind::no_such_slot:
                    return "no such slot";
                case pipeline_error_kind::slot_already_used:
                    return "slot already used";
                case pipeline_error_kind::connection_type_mismatch:
                    return "connection type mismatch";
                default:
                    return "kind not found -- error in the error!!";
            }
        }
        private:
        const pipeline_error_kind kind_;
    };

    enum class poll {
        // A value is available.
        ready,
        // No value is available this time, but there might be one later.
        empty,
        // No value is available, and there never will be again:
        // every future poll for this node will return `poll::closed` again.
        closed,
    };

    class node {
    public:
        virtual auto name() const -> std::string = 0;
        virtual ~node() = default;

    private:
        virtual auto poll_next() -> poll = 0;
        virtual auto set_poll(poll p) -> void = 0;
        virtual void connect(const node* source, int slot) = 0;
        virtual auto slot_exists(int slot) const noexcept -> bool = 0;
        virtual auto all_slots_taken() const noexcept -> bool = 0;
        virtual auto disconnect(node* source) -> void = 0;
        virtual auto get_input_nodes_and_slots() const noexcept -> std::vector<std::pair<const node*, int>> = 0;
        virtual auto get_number_of_inputs() const noexcept -> std::size_t = 0;

        friend class pipeline;
    };

    template <typename Output>
    struct producer : node {
        using output_type = Output;

        virtual auto value() const -> const output_type& = 0;
    private:
        virtual void connect(const node*, int) override {
            return;
        }
        auto disconnect(node*) -> void override {
            return;
        }
        auto set_poll(poll p) -> void override{
            poll_ = p;
        }
        auto slot_exists(int) const noexcept -> bool override {
            return false;
        }
        auto all_slots_taken() const noexcept -> bool override{
            return true;
        }
        auto get_input_nodes_and_slots() const noexcept -> std::vector<std::pair<const node*, int>> override {
            return in_sources_;
        }
        auto get_number_of_inputs() const noexcept -> std::size_t override {
            return in_size_;
        }
        poll poll_ = poll::empty;
        std::string name_ = "Producer";
        static constexpr std::size_t in_size_ = 0;
        std::vector<std::pair<const node*, int>> in_sources_;
    };
    template <>
    struct producer <void>: node {
        using output_type = void;

    private:
        virtual void connect(const node*, int) override {
            return;
        }
        auto disconnect(node*) -> void override {
            return;
        }
        auto set_poll(poll p) -> void override{
            poll_ = p;
        }
        auto slot_exists(int) const noexcept -> bool override {
            return false;
        }
        auto all_slots_taken() const noexcept -> bool override{
            return true;
        }
        auto get_input_nodes_and_slots() const noexcept -> std::vector<std::pair<const node*, int>> override {
            return in_sources_;
        }
        auto get_number_of_inputs() const noexcept -> std::size_t override {
            return in_size_;
        }
        poll poll_ = poll::empty;
        std::string name_ = "Producer";
        static constexpr std::size_t in_size_ = 0;
        std::vector<std::pair<const node*, int>> in_sources_;
    };

    template <typename Input, typename Output>
    struct component : public producer<Output> {
        using input_type = Input;

    private:
        auto slot_exists(int slot) const noexcept -> bool override {
            return slot >= 0 && slot < static_cast<int>(in_size_);
        }
        auto all_slots_taken() const noexcept -> bool override{
            return in_sources_.size() == in_size_;
        }
        auto get_input_nodes_and_slots() const noexcept -> std::vector<std::pair<const node*, int>> override {
            return in_sources_;
        }
        auto get_number_of_inputs() const noexcept -> std::size_t override {
            return in_size_;
        }
        std::string name_ = "Component";
        static constexpr std::size_t in_size_ = std::tuple_size_v<Input>;
        std::vector<std::pair<const node*, int>> in_sources_;
    };

    template <typename Input>
    struct sink : component<std::tuple<Input>, void> {
    };

    template <typename Output>
    struct source : component<std::tuple<>, Output> {
        virtual void connect(const node *, int) noexcept override {};
    };

    template <typename N>
    concept concrete_node = requires {
        typename N::input_type;
        std::same_as<typename N::input_type, typename std::tuple<>>;
        typename N::output_type;
        std::derived_from<N, ppl::producer<typename N::output_type>>;
        std::is_class_v<N>;
        !std::is_abstract_v<N>;
    };

    
    class pipeline {
    public:
        // 3.6.1
        using node_id = int;
        // 3.6.2
        pipeline() : nodes_{}, current_id_{0} {}// must be default constructible
        pipeline(const pipeline &) = delete;
        pipeline(pipeline&& other) : nodes_{other.nodes_}{
            other.nodes_.clear(); // Why does this not work? nodes_ = std::exchange(other.nodes_, std::vector<node> {});
            current_id_ = std::exchange(other.current_id_, 0);
        } // copy constructible
        auto operator=(const pipeline &) -> pipeline& = delete;
        auto operator=(pipeline && other) -> pipeline& {
            if (this == &other) {
                return *this;
            }
            nodes_ = other.nodes_;
            other.nodes_.clear();
            current_id_ = std::exchange(other.current_id_, 0);
            return *this;

        } // copy assignable
        ~pipeline() = default; // if clean up is necessary edit this


        // 3.6.3
        // Function template for inserting into an unordered map (which stores a node's types)
        template <std::size_t index, typename tuple>
        void um_insert_type(auto& um) {
            if constexpr (index == std::tuple_size_v<tuple>) {
                return;
            } else {
                um[index] = typeid(std::tuple_element_t<index, tuple>).hash_code();
                return um_insert_type<index + 1, tuple>(um);
            }
        }

        template <typename N, typename... Args>
        requires concrete_node<N> and std::constructible_from<N, Args...>
        auto create_node(Args&& ...args) -> node_id {
            N n = {args...}; // help me
            N* ptr = &n;

            std::unordered_map<std::size_t, std::size_t> um;
            um_insert_type<0, typename N::input_type>(um);
            
            node_io_types[std::type_index(typeid(N))] = std::make_tuple(um, typeid(typename N::output_type).hash_code());
            auto node_and_id = std::make_tuple(current_id_, ptr);
            nodes_.push_back(node_and_id);
            return current_id_++;
        }
        void erase_node(node_id n_id) {
            auto result = std::find_if(
                nodes_.begin(), 
                nodes_.end(), 
                [n_id](auto &p){
                    return std::get<0>(p) == n_id;
                }
            );
            if (result != nodes_.end()) {
                nodes_.erase(result);
            } else {
                throw pipeline_error(pipeline_error_kind::invalid_node_id);
            }
        }
        auto get_node(node_id n_id) const noexcept -> node* {
            auto result = std::find_if(
                nodes_.begin(), 
                nodes_.end(), 
                [n_id](auto &p){
                    return std::get<0>(p) == n_id;
                }
            );
            if (result != nodes_.end()) {
                return std::get<1>(*result);
            } else {
                return nullptr;
            }
        }

        // 3.6.4
        void connect(node_id src, node_id dst, int slot) {
            // Getting optionals
            auto src_node_info = get_all_node_info(src);
            auto dst_node_info = get_all_node_info(dst);
            
            // Checking if any node_id is invalid
            if (!src_node_info.has_value() || !dst_node_info.has_value()) {
                throw pipeline_error(pipeline_error_kind::invalid_node_id);
            }
            auto src_node = std::get<1>(src_node_info.value());
            auto dst_node = std::get<1>(dst_node_info.value());
            // Checking if node slots all full
            if(dst_node->all_slots_taken()) {
                throw pipeline_error(pipeline_error_kind::slot_already_used);
            }
            // Checking if node slot exists
            if(!dst_node->slot_exists(slot)) {
                throw pipeline_error(pipeline_error_kind::no_such_slot);
            }
            // Getting respective input and output std::type_info
            auto& dst_input_hash = std::get<0>(node_io_types[std::type_index(typeid(dst_node))])[static_cast<std::size_t>(slot)];
            auto& src_output_hash = std::get<1>(node_io_types[std::type_index(typeid(src_node))]);
            // // Checking if output type matches input type
            if (dst_input_hash != src_output_hash) {
                throw pipeline_error(pipeline_error_kind::connection_type_mismatch);
            }
            // If all passes, process the connect
            dst_node->connect(src_node, slot);
        };
        void disconnect(node_id src, node_id dst) {
            // Getting optionals
            auto src_node_info = get_all_node_info(src);
            auto dst_node_info = get_all_node_info(dst);
            
            // Checking if any node_id is invalid
            if (!src_node_info.has_value() || !dst_node_info.has_value()) {
                throw pipeline_error(pipeline_error_kind::invalid_node_id);
            }
            auto src_node = std::get<1>(src_node_info.value());
            auto dst_node = std::get<1>(dst_node_info.value());
            // If all passes, process the disconnect
            dst_node->disconnect(src_node);
        }
        auto get_dependencies(node_id src) const -> std::vector<std::pair<node_id, int>> {
            // Getting optionals
            auto src_node_info = get_all_node_info(src);
            
            // Checking if any node_id is invalid
            if (!src_node_info.has_value()) {
                throw pipeline_error(pipeline_error_kind::invalid_node_id);
            }
            // src_node is the node being depended on
            auto src_node = std::get<1>(src_node_info.value());

            // Setting up result vector
            std::vector<std::pair<node_id, int>> result = {};
            // Searching through every existing node
            for (const auto& n : nodes_) {
                // Getting dependencies for node n
                auto dependencies = std::get<1>(n)->get_input_nodes_and_slots ();
                // Searching through dependencies
                for (const auto& d_node : dependencies) {
                    // If src_node is found
                    if (std::get<0>(d_node) == src_node) {
                        // Construct return pair with current node and slot src_node is in
                        std::pair<node_id, int> return_pair = {std::get<0>(n), std::get<1>(d_node)};
                        result.push_back(return_pair);
                    }
                }
            }
            return result;
        }

        // 3.6.5
        auto is_valid() noexcept -> bool {
            auto contains_sink = false;
            auto contains_source = false;

            std::vector<std::tuple<int, node*>> sinks;

            // Go through every node
            for (auto& n : nodes_) {
                // If any node doesn't have all source slots filled
                if (!std::get<1>(n)->all_slots_taken()) {
                    return false;
                }
                auto dependencies = get_dependencies(std::get<0>(n));
                // If no dependencies and not sink node
                if (dependencies.size() == 0) {// && !sink
                    return false;
                }
                // Getting output type
                auto & n_io_pair = node_io_types[std::type_index(typeid(std::get<1>(n)))];
                auto & n_output_type_hash = std::get<1>(n_io_pair);
                if (n_output_type_hash == typeid(void).hash_code()) {
                    contains_sink = true;
                    sinks.emplace_back(n);
                }
                // Getting number of inputs
                const auto & input_type_um_size = std::get<1>(n)->get_number_of_inputs();
                if (input_type_um_size == 0) {
                    contains_source = true;
                }
            }

            // Checking for sub-pipelines
            std::queue<std::tuple<node_id, node*>> q;
            std::vector<std::tuple<node_id, node*>> seen;
            if (!nodes_.empty()) {
                q.push(nodes_.at(0));
                seen.push_back(nodes_.at(0));
            }
            
            std::tuple<node_id, node*> n;
            std::tuple<node_id, node*> n_temp;
            std::vector<std::pair<node_id, int>> dependencies;
            std::vector<std::pair<const node*, int>> inputs;
            // Do breadth first search starting from a random node
            // Add every incoming/outgoing edge to q
            while (!q.empty()) {
                // Dequeue first element
                n = q.front();
                q.pop();
                // Get n's dependants
                dependencies = get_dependencies(std::get<0>(n));
                for (const auto& d_info : dependencies) {
                    // Get dependant node
                    n_temp = get_all_node_info(std::get<0>(d_info)).value();
                    // If not seen (and thereby not in queue)
                    if (std::find(seen.begin(), seen.end(), n_temp) != seen.end()) {
                        q.push(n_temp);
                        seen.push_back(n_temp);
                    }
                }
                // Get n's inputs
                inputs = std::get<1>(n)->get_input_nodes_and_slots();
                for (const auto& i_info : inputs) {
                    // Get input node
                    n_temp = get_all_node_info(std::get<1>(i_info)).value();
                    // If not seen (and thereby not in queue)
                    if (std::find(seen.begin(), seen.end(), n_temp) != seen.end()) {
                        q.push(n_temp);
                        seen.push_back(n_temp);
                    }
                }
            }
            // Goes through seen list
            for (const auto& node : seen) {
                // If any node isn't found, then graph is disconnected
                if (std::find(nodes_.begin(), nodes_.end(), node) == nodes_.end()) {
                    return false;
                }
            }
            
            // Checking for cycles
            seen.clear();
            // Do DFS keeping track of back-edges connecting to edges in recursion stack
            // Make sure to reverse edges to get the entire graph
            
            return true && contains_sink && contains_source;
        }
        auto step() -> bool {
            // Create function that sets all dependent nodes to closed
            // Create function that polls all dependent nodes
            // Have another branch that skips the current node
            // Write logic to direct flow between these three branches after polling current node
            return false;
        }
        void run() {
            while(!step()){}
        }

        // 3.6.6
        friend std::ostream &operator<<(std::ostream &stream, const ppl::pipeline &line) {

        auto nodes = line.nodes_;
        // Sort nodes by ascending id
        std::sort(
            nodes.begin(), 
            nodes.end(), 
            [](const auto& n1, const auto& n2) {
                return std::get<0>(n1) < std::get<0>(n2);
            }
        );

        // Starting line
        stream << static_cast<std::string>("digraph G {\n");

        // Goes through every node
        for (const auto& n : nodes) {
            // Creating string s in form `${id} ${name}`
            std::string s = std::to_string(std::get<0>(n)) + " " + std::get<1>(n)->name();
            stream << "  \"" + s + "\"\n";
        }
        std::string s = "\n";
        stream << s;
        // Getting edges in form:
        // `  "${src_node_id} ${src_node_name}" -> "${dst_node_id} ${dst_node_name}"`
        std::vector<std::string> edges = {};
        // Goes through every node
        for (const auto& n : nodes) {
            // String for node n (start of edge)
            std::string s1 = std::to_string(std::get<0>(n)) + " " + std::get<1>(n)->name();
            // Getting dependencies
            auto inputs = line.get_input_nodes_and_slots(std::get<1>(n));
            // Going through dependencies
            for (const auto& input : inputs) {
                // Finding dependant node in all existing nodes to get node_id (annoying I know)
                auto result = std::find_if(
                    nodes.begin(), 
                    nodes.end(), 
                    [input](auto &p){
                        return std::get<1>(p) == std::get<0>(input);
                    }
                );
                // String for dependant node (end of edge)
                std::string s2 = std::to_string(std::get<0>(*result)) + " " + std::get<1>(*result)->name();
                // Pushing to edges vector
                edges.emplace_back("  \"" + s1 + "\" -> \"" + s2 + "\"\n");
            }
        }
        // Sorting lexicographically (which will sort it by id)
        std::sort(edges.begin(), edges.end());

        // Printing all edges
        for (const auto& e : edges) {
            stream << e;
        }


        // Ending line
        stream << static_cast<std::string>("}\n");
        return stream;
    }

    private:
        std::vector<std::tuple<node_id, node*>> nodes_;
        node_id current_id_;
        std::unordered_map<std::type_index, std::tuple<std::unordered_map<std::size_t, std::size_t>, std::size_t>> node_io_types;

        auto get_all_node_info(node_id n_id) const noexcept -> std::optional<std::tuple<node_id, node*>> {
            auto result = std::find_if(
                nodes_.begin(), 
                nodes_.end(), 
                [n_id](auto &p){
                    return std::get<0>(p) == n_id;
                }
            );
            if (result != nodes_.end()) {
                return std::optional<std::tuple<node_id, node*>>(*result);
            } else {
                return std::optional<std::tuple<node_id, node*>>();
            }
        }
        auto get_input_nodes_and_slots(node* input) const noexcept -> std::vector<std::pair<const node*, int>> {
            return input->get_input_nodes_and_slots();
        }
    };




}

#endif  // COMP6771_PIPELINE_H
