# include <iostream>
# include <vector>
# include <queue>
# include <cstdio>

template<typename TEdgeData>
class Edge {
public:
    Edge() = default;
    Edge(const Edge& other):
        beginning_(other.beginning_), destination_(other.destination_),
        capacity_(other.capacity_), pair_edge_(other.pair_edge_), data(other.data, this) {}
    Edge(Edge&& other) noexcept:
        beginning_(std::move(other.beginning_)), destination_(std::move(other.destination_)),
        capacity_(std::move(other.capacity_)), pair_edge_(std::move(other.pair_edge_)),
        data(std::move(other.data), this) {}

    Edge(size_t begin, size_t end, long long capacity = 0,
         size_t pair = 0, Edge* reverse = nullptr) :
        beginning_(begin),
        destination_(end), capacity_(capacity), pair_edge_(pair), data(this, reverse) {}

    Edge& operator=(const Edge&) = default;
    Edge& operator=(Edge&& other) noexcept {
        beginning_ = std::move(other.beginning_);
        destination_ = std::move(other.destination_);
        capacity_ = std::move(other.capacity_);
        pair_edge_ = std::move(other.pair_edge_);
        data = TEdgeData(std::move(other.data), this);
        return *this;
    }

    size_t getBegin() const {
        return beginning_;
    }

    size_t getDest() const {
        return destination_;
    }

    TEdgeData& getData() const {
        return data;
    }

    size_t getPair() const {
        return pair_edge_;
    }

    long long getCapacity() const {
        return capacity_;
    }

    ~Edge() = default;

private:
    size_t beginning_, destination_;
    long long capacity_;
    size_t pair_edge_;
    mutable TEdgeData data;
};

template<typename TEdgeData>
class EdgesIterator;

template<typename TEdgeData>
class INetwork {
public:
    class EdgesIterator_ {
    public:
        virtual EdgesIterator_* next() const = 0;
        virtual EdgesIterator_* prev() const = 0;
        virtual EdgesIterator_* backEdge() const = 0;
        virtual EdgesIterator_* clone() const = 0;
        virtual size_t getBeginning() const = 0;
        virtual size_t getDestination() const = 0;
        virtual typename TEdgeData::FlowT getCapacity() const = 0;
        virtual TEdgeData& edgeData() const = 0;
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual bool equal(const EdgesIterator_* other) const = 0;
        virtual ~EdgesIterator_() = 0;
    };
    virtual EdgesIterator<TEdgeData> beginIter(size_t vertex) const = 0;
    virtual EdgesIterator<TEdgeData> endIter(size_t vertex) const = 0;
    virtual size_t getSource() const = 0;
    virtual size_t getSink() const = 0;
    virtual size_t getSize() const = 0;
    virtual ~INetwork() = 0;
    static const typename TEdgeData::FlowT INF;
};
template<typename TEdgeData>
INetwork<TEdgeData>::~INetwork() {}
template<typename TEdgeData>
INetwork<TEdgeData>::EdgesIterator_::~EdgesIterator_() {}
template<typename TEdgeData>
const typename TEdgeData::FlowT INetwork<TEdgeData>::INF = 1e9 + 9;

template<typename TEdgeData>
class EdgesIterator {
public:
    EdgesIterator() = default;

    EdgesIterator(const EdgesIterator& other) : it_(other.it_->clone()) {}

    EdgesIterator(EdgesIterator&& other) noexcept : it_(other.it_) {
        other.it_ = nullptr;
    }

    EdgesIterator(typename INetwork<TEdgeData>::EdgesIterator_* it) : it_(it) {}

    EdgesIterator& operator=(const EdgesIterator& other) {
        if (it_ != nullptr)
            delete it_;
        it_ = other.it_->clone();
        return *this;
    }

    EdgesIterator& operator=(EdgesIterator&& other) noexcept {
        if (it_ != nullptr)
            delete it_;
        it_ = other.it_;
        other.it_ = nullptr;
        return *this;
    }

    ~EdgesIterator() {
        if (it_ != nullptr)
            delete it_;
    }

    bool operator==(const EdgesIterator& other) const {
        return it_->equal(other.it_);
    }

    bool operator!=(const EdgesIterator& other) const {
        return !operator==(other);
    }

    EdgesIterator& operator++() {
        it_->increment();
        return *this;
    }

    EdgesIterator& operator--() {
        it_->decrement();
        return *this;
    }

    EdgesIterator operator++(int) {
        EdgesIterator cpy = *this;
        it_->increment();
        return cpy;
    }

    EdgesIterator operator--(int) {
        EdgesIterator cpy = *this;
        it_->decrement();
        return cpy;
    }

    EdgesIterator backEdge() const {
        return EdgesIterator(it_->backEdge());
    }

    size_t getBeginning() const {
        return it_->getBeginning();
    }

    size_t getDestination() const {
        return it_->getDestination();
    }

    typename TEdgeData::FlowT getCapacity() const {
        return it_->getCapacity();
    }

    TEdgeData& edgeData() const {
        return it_->edgeData();
    }
private:
    typename INetwork<TEdgeData>::EdgesIterator_* it_;
};

class EdgeData {
public:
    typedef long long int FlowT;

    EdgeData() = default;

    EdgeData(const EdgeData&) = default;

    EdgeData(const EdgeData& other, Edge<EdgeData>* edge):
          flow_(other.flow_), edge_(edge), back_edge_(other.back_edge_) {
        if (back_edge_ != nullptr)
            back_edge_->getData().back_edge_ = edge_;
    }

    EdgeData(EdgeData&& other, Edge<EdgeData>* edge) noexcept:
          flow_(std::move(other.flow_)), edge_(edge),
          back_edge_(std::move(other.back_edge_)) {
        if (back_edge_ != nullptr) {
            back_edge_->getData().back_edge_ = edge_;
        }
    }

    EdgeData(Edge<EdgeData>* edge, Edge<EdgeData>* back_edge, FlowT flow = FlowT()):
        flow_(flow), edge_(edge), back_edge_(back_edge) {}

    FlowT getResidualCapacity() const {
        return edge_->getCapacity() - flow_;
    }

    bool saturated() const {
        return (getResidualCapacity() == 0);
    }

    FlowT getFlow() const {
        return flow_;
    }

    void addFlow(FlowT cap) {
        flow_ += cap;
        back_edge_->getData().flow_ -= cap;
    }

    ~EdgeData() = default;
private:
    FlowT flow_;
    Edge<EdgeData>* edge_, *back_edge_;
};

template<typename TEdgeData>
class Network : public INetwork<TEdgeData> {
public:
    Network() = default;

    Network(const Network&) = default;

    Network(Network&&) = default;

    Network(size_t n, size_t s, size_t t) : graph_(n), s_(s), t_(t) {}

    Network& operator=(const Network&) = default;

    Network& operator=(Network&&) = default;

    ~Network() = default;

    void addEdge(size_t u, size_t v, long long capacity) {
        graph_[u].push_back(Edge<TEdgeData>());
        graph_[v].push_back(Edge<TEdgeData>());
        if (u == v) {
            graph_[u][graph_[u].size() - 2] = Edge<TEdgeData>(u, v, capacity,
                                                              graph_[u].size() - 1,
                                                              &graph_[v].back());
            graph_[u].back() = Edge<TEdgeData>(v, u, 0, graph_[u].size() - 2,
                                               &graph_[u][graph_[u].size() - 2]);
            return;
        }
        graph_[u].back() = Edge<TEdgeData>(u, v, capacity,
                                           graph_[v].size() - 1, &graph_[v].back());
        graph_[v].back() = Edge<TEdgeData>(v, u, 0,
                                           graph_[u].size() - 1, &graph_[u].back());
    }

    size_t getSource() const override {
        return s_;
    }

    size_t getSink() const override {
        return t_;
    }

    size_t getSize() const override {
        return graph_.size();
    }

    class EdgesIterator_ : public INetwork<TEdgeData>::EdgesIterator_ {
    public:
        EdgesIterator_() = default;

        EdgesIterator_(int vertex, const Network* netw, int edge = 0):
            netw_(netw), vertex_(vertex), edge_(edge) {}

        ~EdgesIterator_() = default;

        typename INetwork<TEdgeData>::EdgesIterator_* next() const override {
            return new EdgesIterator_(vertex_, netw_, edge_ + 1);
        }

        typename INetwork<TEdgeData>::EdgesIterator_* prev() const override {
            return new EdgesIterator_(vertex_, netw_, edge_ - 1);
        }

        typename INetwork<TEdgeData>::EdgesIterator_* backEdge() const override {
            return new EdgesIterator_(netw_->graph_[vertex_][edge_].getDest(), netw_,
                                     netw_->graph_[vertex_][edge_].getPair());
        }

        typename INetwork<TEdgeData>::EdgesIterator_* clone() const override {
            return new EdgesIterator_(vertex_, netw_, edge_);
        }

        TEdgeData& edgeData() const override {
            return netw_->graph_[vertex_][edge_].getData();
        }

        size_t getBeginning() const override {
            return netw_->graph_[vertex_][edge_].getBegin();
        }

        typename TEdgeData::FlowT getCapacity() const override {
            return netw_->graph_[vertex_][edge_].getCapacity();
        }

        size_t getDestination() const override {
            return netw_->graph_[vertex_][edge_].getDest();
        }

        void increment() override {
            ++edge_;
        }

        void decrement() override {
            --edge_;
        }

        bool equal(const typename INetwork<TEdgeData>::EdgesIterator_* other) const override {
            return (edge_ == dynamic_cast<const EdgesIterator_*>(other)->edge_);
        }
    private:
        const Network* netw_;
        int vertex_, edge_;
    };

    EdgesIterator<TEdgeData> beginIter(size_t vertex) const override {
        return EdgesIterator<TEdgeData>(new EdgesIterator_(vertex, this));
    }

    EdgesIterator<TEdgeData> endIter(size_t vertex) const override {
        return EdgesIterator<TEdgeData>(new EdgesIterator_(vertex, this,
                                        static_cast<int>(graph_[vertex].size())));
    }
private:
    std::vector<std::vector<Edge<TEdgeData>>> graph_;
    size_t s_, t_;
};

template<typename TEdgeData>
class MKMAlgorithm {
public:
    ~MKMAlgorithm() = default;

    static typename TEdgeData::FlowT blockingFlow(const INetwork<TEdgeData>* netw,
                                                  const std::vector<size_t>& dist) {
        MKMAlgorithm alg(netw, dist);
        typename TEdgeData::FlowT ans = alg.calculateBlockingFlow();
        return ans;
    }
private:
    MKMAlgorithm(const INetwork<TEdgeData>* netw, const std::vector<size_t>& dist):
         pot_in_(netw->getSize()), pot_out_(netw->getSize()), excess_(netw->getSize()),
         current_edge_(netw->getSize()), current_edge_back_(netw->getSize()),
         dist_(dist), netw_(netw) {
        for (size_t i = 0; i < netw->getSize(); ++i) {
            if (dist_[i] == INetwork<TEdgeData>::INF)
                continue;
            current_edge_[i] = netw_->beginIter(i);
            current_edge_back_[i] = netw_->beginIter(i);
            for (auto it = netw->beginIter(i); it != netw->endIter(i); ++it) {
                if (dist_[it.getDestination()] + 1 == dist_[i])
                    pot_in_[i] += it.backEdge().edgeData().getResidualCapacity();
                else if (dist_[i] + 1 == dist_[it.getDestination()])
                    pot_out_[i] += it.edgeData().getResidualCapacity();
            }
        }
    }

    typename TEdgeData::FlowT getPotential(size_t vertex) const {
        if (vertex == netw_->getSource())
            return pot_out_[vertex];
        else if (vertex == netw_->getSink())
            return pot_in_[vertex];
        return std::min(pot_in_[vertex], pot_out_[vertex]);
    }

    std::pair<typename TEdgeData::FlowT, size_t> minPotentialVertex() const {
        typename TEdgeData::FlowT min_pot = std::min(pot_out_[netw_->getSource()],
                                                     pot_in_[netw_->getSink()]);
        if (min_pot == 0)
            return {min_pot, netw_->getSink()};

        size_t vert = (pot_out_[netw_->getSource()] < pot_in_[netw_->getSink()] ?
                       netw_->getSource() : netw_->getSink());

        for (size_t i = 0; i < netw_->getSize(); ++i) {
            if (pot_in_[i] != 0 && pot_out_[i] != 0 && min_pot > getPotential(i)) {
                min_pot = getPotential(i);
                vert = i;
            }
        }
        return {min_pot, vert};
    }

    template<typename TFunc1, typename TFunc2, typename TFunc3>
    void pushExcess(std::vector<EdgesIterator<TEdgeData>>& curr_edge,
                    size_t vertex, size_t target, const typename TEdgeData::FlowT& min_pot,
                    std::vector<typename TEdgeData::FlowT>& nxt,
                    std::vector<typename TEdgeData::FlowT>& prv,
                    TFunc1 good_edge, TFunc2 get_data, TFunc3 get_dest) {
        std::fill(excess_.begin(), excess_.end(), typename TEdgeData::FlowT());
        excess_[vertex] = min_pot;
        std::queue<size_t> vertices;
        vertices.push(vertex);
        while (!vertices.empty()) {
            size_t curr = vertices.front();
            vertices.pop();
            if (curr == target)
                break;
            while (excess_[curr] != 0 &&
                   curr_edge[curr] != netw_->endIter(curr)) {
                if (good_edge(curr_edge, curr)) {
                    TEdgeData& data = get_data(curr_edge, curr);
                    size_t dest = get_dest(curr_edge, curr);
                    typename TEdgeData::FlowT pushing = std::min(data.getResidualCapacity(),
                                                                 excess_[curr]);
                    excess_[curr] -= pushing;
                    nxt[curr] -= pushing;
                    excess_[dest] += pushing;
                    prv[dest] -= pushing;
                    if (excess_[dest] == pushing && pushing != 0)
                        vertices.push(dest);
                    data.addFlow(pushing);
                    if (data.saturated())
                        ++curr_edge[curr];
                }
                else
                    ++curr_edge[curr];
            }
        }
    }

    void deleteVertices() {
        std::queue<size_t> vertices;
        std::vector<bool> used(netw_->getSize());
        for (size_t i = 0; i < netw_->getSize(); ++i) {
            if (getPotential(i) == 0) {
                vertices.push(i);
                used[i] = true;
            }
        }
        while (!vertices.empty()) {
            size_t curr_vertex = vertices.front();
            vertices.pop();
            if (curr_vertex == netw_->getSource() || curr_vertex == netw_->getSink())
                break;
            for (auto it = netw_->beginIter(curr_vertex); it != netw_->endIter(curr_vertex); ++it) {
                if (!it.edgeData().saturated() &&
                    dist_[it.getDestination()] == dist_[curr_vertex] + 1 &&
                    !used[it.getDestination()]) {
                    pot_in_[it.getDestination()] -= it.edgeData().getResidualCapacity();
                    if (pot_in_[it.getDestination()] == 0 && !used[it.getDestination()]) {
                        used[it.getDestination()] = true;
                        vertices.push(it.getDestination());
                    }
                }
                else if (!it.backEdge().edgeData().saturated() &&
                         dist_[it.getDestination()] + 1 == dist_[curr_vertex] &&
                         !used[it.getDestination()]) {
                    pot_out_[it.getDestination()] -= it.backEdge().edgeData().getResidualCapacity();
                    if (pot_out_[it.getDestination()] == 0 && !used[it.getDestination()]) {
                        used[it.getDestination()] = true;
                        vertices.push(it.getDestination());
                    }
                }
            }
            dist_[curr_vertex] = INetwork<TEdgeData>::INF;
        }
    }

    typename TEdgeData::FlowT calculateBlockingFlow() {
        typename TEdgeData::FlowT ans = typename TEdgeData::FlowT();
        while (true) {
            auto min_pot_data = minPotentialVertex();
            typename TEdgeData::FlowT min_pot = min_pot_data.first;
            size_t start_vertex = min_pot_data.second;
            if (min_pot == 0)
                break;
            ans += min_pot;

            pushExcess(current_edge_, start_vertex, netw_->getSink(), min_pot, pot_out_, pot_in_,
                       [this](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) {
                             return !edge[vert].edgeData().saturated() &&
                                    dist_[vert] + 1 == dist_[edge[vert].getDestination()];
                       }, [](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) ->
                          TEdgeData& {
                              return edge[vert].edgeData();
                       }, [](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) {
                              return edge[vert].getDestination();
                       });

            pushExcess(current_edge_back_, start_vertex, netw_->getSource(), min_pot, pot_in_, pot_out_,
                       [this](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) {
                             return !edge[vert].backEdge().edgeData().saturated() &&
                                    dist_[vert] == 1 + dist_[edge[vert].getDestination()];
                       }, [](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) ->
                          TEdgeData& {
                              return edge[vert].backEdge().edgeData();
                       }, [](std::vector<EdgesIterator<TEdgeData>>& edge, size_t vert) {
                              return edge[vert].getDestination();
                       });

            deleteVertices();
        }
        return ans;
    }

    std::vector<typename TEdgeData::FlowT> pot_in_, pot_out_, excess_;
    std::vector<EdgesIterator<TEdgeData>> current_edge_, current_edge_back_;
    std::vector<size_t> dist_;
    const INetwork<TEdgeData>* netw_;
};


template<template<typename> typename BlockFlowAlgo>
class BlockingFlowFinder {
public:
    template<typename TEdgeData>
    static void setLayerNetwork(const INetwork<TEdgeData>* netw, std::vector<size_t>& dist) {
        setDist_(netw, dist);
        if (dist[netw->getSink()] == INetwork<TEdgeData>::INF)
            return;
        std::vector<bool> reachable = getReachable_(netw, dist, netw->getSink());
        for (size_t i = 0; i < netw->getSize(); ++i) {
            if (!reachable[i])
                dist[i] = INetwork<TEdgeData>::INF;
        }
    }

    template<typename TEdgeData>
    static typename TEdgeData::FlowT maxFlow(const INetwork<TEdgeData>* netw) {
        std::vector<size_t> distance(netw->getSize());
        setLayerNetwork(netw, distance);
        typename TEdgeData::FlowT ans = typename TEdgeData::FlowT();
        while (distance[netw->getSink()] < INetwork<TEdgeData>::INF) {
            ans += BlockFlowAlgo<TEdgeData>::blockingFlow(netw, distance);
            setLayerNetwork(netw, distance);
        }
        return ans;
    }
private:
    template<typename TEdgeData>
    static void setDist_(const INetwork<TEdgeData>* netw, std::vector<size_t>& dist) {
        std::fill(dist.begin(), dist.end(), INetwork<TEdgeData>::INF);
        dist[netw->getSource()] = 0;
        std::queue<size_t> order;
        order.push(netw->getSource());
        while (!order.empty()) {
            size_t curr = order.front();
            order.pop();
            for (auto it = netw->beginIter(curr); it != netw->endIter(curr); ++it) {
                if (!it.edgeData().saturated()
                    && dist[it.getDestination()] == INetwork<TEdgeData>::INF) {
                    dist[it.getDestination()] = dist[it.getBeginning()] + 1;
                    order.push(it.getDestination());
                }
            }
        }
    }

    template<typename TEdgeData>
    static std::vector<bool> getReachable_(const INetwork<TEdgeData>* netw, std::vector<size_t>& dist,
                                           size_t vertex) {
        std::vector<bool> reachable(netw->getSize());
        reachable[vertex] = true;
        std::queue<size_t> order;
        order.push(vertex);
        while (!order.empty()) {
            size_t curr_vertex = order.front();
            order.pop();
            for (auto it = netw->beginIter(curr_vertex); it != netw->endIter(curr_vertex); ++it) {
                if (!it.backEdge().edgeData().saturated() &&
                    dist[it.getDestination()] + 1 == dist[curr_vertex] &&
                    !reachable[it.getDestination()]) {
                    reachable[it.getDestination()] = true;
                    order.push(it.getDestination());
                }
            }
        }
        return reachable;
    }
};

template<typename TEdgeData>
class PushPreflowAlgorithm {
public:
    ~PushPreflowAlgorithm() = default;

    static typename TEdgeData::FlowT maxFlow(const INetwork<TEdgeData>* netw) {
        PushPreflowAlgorithm alg(netw);
        alg.setStart();
        while (alg.validOperations()) {
            for (size_t v = 0; v < netw->getSize(); ++v) {
                if (v != netw->getSource() && v != netw->getSink())
                    alg.discharge(v);
            }
        }
        return alg.getOutFlow(netw->getSource());
    }
private:
    PushPreflowAlgorithm(const INetwork<TEdgeData>* netw):
        current_edge_(netw->getSize()), excess_(netw->getSize()),
        height_(netw->getSize()), netw_(netw) {}

    typename TEdgeData::FlowT getOutFlow(size_t vertex) {
        typename TEdgeData::FlowT ans = typename TEdgeData::FlowT();
        for (auto it = netw_->beginIter(vertex); it != netw_->endIter(vertex); ++it) {
            if (it.getCapacity() > 0)
                ans += it.edgeData().getFlow();
        }
        return ans;
    }

    bool validOperations() const {
        for (size_t i = 0; i < netw_->getSize(); ++i) {
            if (excess_[i] > 0 && i != netw_->getSource() && i != netw_->getSink())
                return true;
        }
        return false;
    }

    void setStart() {
        for (size_t i = 0; i < netw_->getSize(); ++i)
            current_edge_[i] = netw_->beginIter(i);
        std::fill(height_.begin(), height_.end(), 0);
        std::fill(excess_.begin(), excess_.end(), 0);
        height_[netw_->getSource()] = netw_->getSize();
        for (auto it = netw_->beginIter(netw_->getSource());
             it != netw_->endIter(netw_->getSource()); ++it) {
            it.edgeData().addFlow(it.getCapacity());
            excess_[it.getDestination()] += it.getCapacity();
            excess_[it.getBeginning()] -= it.getCapacity();
        }
    }

    void push(const EdgesIterator<TEdgeData>& it) {
        auto delta = std::min(excess_[it.getBeginning()], it.edgeData().getResidualCapacity());
        it.edgeData().addFlow(delta);
        excess_[it.getBeginning()] -= delta;
        excess_[it.getDestination()] += delta;
    }

    void relabel(size_t vertex) {
        size_t new_height = INF_HEIGHT;
        for (auto it = netw_->beginIter(vertex); it != netw_->endIter(vertex); ++it) {
            if (!it.edgeData().saturated())
                new_height = std::min(new_height, height_[it.getDestination()]);
        }
        height_[vertex] = new_height + 1;
    }

    void discharge(size_t vertex) {
        while (excess_[vertex] > 0) {
            if (current_edge_[vertex] == netw_->endIter(vertex)) {
                relabel(vertex);
                current_edge_[vertex] = netw_->beginIter(vertex);
            }
            else {
                if (current_edge_[vertex].edgeData().getResidualCapacity() > 0 &&
                    height_[vertex] == height_[current_edge_[vertex].getDestination()] + 1) {
                    push(current_edge_[vertex]);
                }
                else
                    ++current_edge_[vertex];
            }
        }
    }

    static const size_t INF_HEIGHT;

    std::vector<EdgesIterator<TEdgeData>> current_edge_;
    std::vector<typename TEdgeData::FlowT> excess_;
    std::vector<size_t> height_;
    const INetwork<TEdgeData>* netw_;
};
template<typename TEdgeData>
const size_t PushPreflowAlgorithm<TEdgeData>::INF_HEIGHT = 1e9 + 7;

class Data {
public:
    Data() = default;

    Data(const Data&) = default;

    Data(Data&&) = default;

    Data(size_t n, const std::vector<long long>& utility,
         const std::vector<std::vector<size_t>> needed):
        needed_(needed), utility_(utility), n_(n) {}

    Data& operator=(const Data&) = default;

    Data& operator=(Data&&) = default;

    ~Data() = default;

    const std::vector<std::vector<size_t>>& getNeeded() const {
        return needed_;
    }

    const std::vector<long long>& getUtility() const {
        return utility_;
    }

    size_t getSubjects() const {
        return n_;
    }

    friend std::istream& operator>>(std::istream& in, Data& data);
private:
    std::vector<std::vector<size_t>> needed_;
    std::vector<long long> utility_;
    size_t n_;
};

std::istream& operator>>(std::istream& in, Data& data) {
    in >> data.n_;
    data.needed_.resize(data.n_);
    data.utility_.resize(data.n_);
    for (size_t i = 0; i < data.n_; ++i)
        in >> data.utility_[i];
    for (size_t i = 0; i < data.n_; ++i) {
        size_t sz;
        in >> sz;
        data.needed_[i].resize(sz);
        for (size_t j = 0; j < sz; ++j) {
            in >> data.needed_[i][j];
            --data.needed_[i][j];
        }
    }
    return in;
}

template<typename Algorithm>
class ResultInfo {
public:
    ResultInfo(const Data& data):
          netw_(data.getSubjects() + 2, data.getSubjects(), data.getSubjects() + 1),
          answer_() {
        for (size_t i = 0; i < data.getSubjects(); ++i) {
            for (size_t j : data.getNeeded()[i])
                netw_.addEdge(i, j, INF_EDGE);

            auto ut = data.getUtility()[i];
            if (ut < 0)
                netw_.addEdge(i, data.getSubjects() + 1, -ut);
            else {
                answer_ += ut;
                netw_.addEdge(data.getSubjects(), i, ut);
            }
        }
    }

    void executeAlgorithm() {
        answer_ -= Algorithm::maxFlow(&netw_);
    }

    template<typename TAlgorithm>
    friend std::ostream& operator<<(std::ostream& out, ResultInfo<TAlgorithm> &ri);
private:
    static const typename EdgeData::FlowT INF_EDGE;

    Network<EdgeData> netw_;
    typename EdgeData::FlowT answer_;
};
template<typename Algorithm>
const typename EdgeData::FlowT ResultInfo<Algorithm>::INF_EDGE = 1e10 + 1;

template<typename Algorithm>
std::ostream& operator<<(std::ostream& out, ResultInfo<Algorithm>& ri) {
    out << ri.answer_ << std::endl;
    return out;
}

template<typename Algorithm>
ResultInfo<Algorithm> run(const Data& data) {
    ResultInfo<Algorithm> ans(data);
    ans.executeAlgorithm();
    return ans;
}

template<typename Algorithm>
void process(std::istream& in, std::ostream& out) {
    Data data;
    in >> data;
    ResultInfo<Algorithm> ans = run<Algorithm>(data);
    out << ans;
}

int main() {
    // process<PushPreflowAlgorithm<EdgeData>>(std::cin, std::cout);
    process<BlockingFlowFinder<MKMAlgorithm>>(std::cin, std::cout);
    return 0;
}
