# include <iostream>
# include <string>
# include <vector>
# include <cinttypes>
# include <map>
# include <algorithm>
# include <queue>

template<typename T>
class SuffixTree;

template<typename T>
class Node;

template<typename T>
class TreeVisitor;

template<typename T>
class Position {
public:
    Position() = default;

    Position(const Position&) = default;
    Position& operator = (const Position&) = default;

    Position(Position&& other) noexcept : node_(other.node_), dist_(other.dist_) {
        other.node_ = nullptr;
    }

    Position& operator = (Position&& other) noexcept {
        node_ = other.node_;
        dist_ = other.dist_;
        other.node_ = nullptr;
        return *this;
    }

    explicit Position(Node<T>* node, ssize_t dist = 0) : node_(node), dist_(dist) {}

    bool isNode() const {
        return (dist_ == 0);
    }

    template<typename U>
    friend class Node;

    template<typename U>
    friend class SuffixTree;

    template<typename U>
    friend class TreeVisitor;
private:
    Node<T>* node_;
    ssize_t dist_;
};

template<typename T>
class Node {
public:
    Node() = default;

    Node(const Node&) = delete;
    Node& operator = (const Node&) = delete;

    Node(Node&& other) noexcept :
                         children_(std::move(other.children_)),
                         edge_start_(other.edge_start_),
                         edge_finish_(other.edge_finish_),
                         link_(other.link_), parent_(other.parent_) {
        other.link_ = nullptr, other.parent_ = nullptr;
    }

    ~Node() {
        for (auto child : children_)
            delete child.second;
    }

    bool isLeaf() const {
        return children_.empty();
    }

    size_t getLength() const {
        return edge_finish_ - edge_start_;
    }

    template<typename U>
    friend class SuffixTree;

    template<typename U>
    friend class TreeVisitor;
private:
    Node(const std::map<T, Node*> chd, ssize_t start, ssize_t finish, Node* link, Node* par):
        children_(chd), edge_start_(start), edge_finish_(finish), link_(link), parent_(par) {}

    std::map<T, Node*> children_;
    ssize_t edge_start_, edge_finish_;
    Node* link_, *parent_;
};

template<typename T>
class SuffixTree {
public:
    SuffixTree() : root_(new Node<T>), preroot_(new Node<T>), last_not_leaf_(root_, 0) {
        root_->edge_start_ = -1, root_->edge_finish_ = 0;
        root_->link_ = preroot_, root_->parent_ = preroot_;
    }

    ~SuffixTree() {
        delete preroot_;
        delete root_;
    }

    void add(const T& symb) {
        string_.push_back(symb);
        while (!canGo(last_not_leaf_, symb)) {
            Node<T>* node = makeNode(last_not_leaf_);
            node->children_[symb] = new Node<T>(std::map<T, Node<T>*>(), string_.size() - 1,
                                                INF, nullptr, node);
            last_not_leaf_ = Position<T>(getLink(node));
        }
        last_not_leaf_ = go(last_not_leaf_, symb);
    }

    template<typename U>
    friend class TreeVisitor;
private:
    Node<T>* makeNode(Position<T> pos) {
        if (pos.isNode())
            return pos.node_;
        Node<T>* node = new Node<T>(std::map<T, Node<T>*>(), pos.node_->edge_start_,
                                    pos.node_->edge_finish_ - pos.dist_,
                                    nullptr, pos.node_->parent_);
        node->children_[string_[node->edge_finish_]] = pos.node_;
        node->parent_->children_[string_[node->edge_start_]] = node;
        pos.node_->parent_ = node;
        pos.node_->edge_start_ = node->edge_finish_;
        return node;
    }

    Position<T> getEdgePosition(Node<T>* node, const T& symb) const {
        Node<T>* vertex = (node != preroot_ ? node->children_[symb] : root_);
        ssize_t dst = vertex->edge_finish_ - vertex->edge_start_;
        return Position<T>(vertex, dst);
    }

    Node<T>* buildLink(Node<T>* node) {
        Node<T>* parent = node->parent_;
        Position<T> pos(getLink(parent));
        ssize_t start = node->edge_start_;
        ssize_t finish = node->edge_finish_;
        while (start < finish) {
            if (pos.isNode())
                pos = getEdgePosition(pos.node_, string_[start]);
            ssize_t len = std::min(finish - start, pos.dist_);
            pos.dist_ -= len;
            start += len;
        }
        return makeNode(pos);
    }

    Node<T>* getLink(Node<T>* node) {
        if (node->link_ == nullptr)
            node->link_ = buildLink(node);
        return node->link_;
    }

    Position<T> go(Position<T> pos, const T& symb) const {
        if (pos.node_ == preroot_)
            return Position<T>(root_);
        if (pos.isNode())
            pos = getEdgePosition(pos.node_, symb);
        --pos.dist_;
        return pos;
    }

    bool canGo(Position<T> pos, const T& symb) const {
        if (pos.node_ == preroot_) {
            return true;
        }
        if (pos.isNode())
            return (pos.node_->children_.find(symb) != pos.node_->children_.end());
        return (string_[pos.node_->edge_finish_ - pos.dist_] == symb);
    }

    static const ssize_t INF = 1e9 + 7;

    std::vector<T> string_;
    Node<T>* root_, *preroot_;
    Position<T> last_not_leaf_;
};

template<typename T, typename NodeT, typename VisitorT>
class IChildrenIterator : private std::map<T, NodeT*>::const_iterator {
public:
    bool operator == (const IChildrenIterator& other) const {
        return wrappedType_::operator == (static_cast<wrappedType_>(other));
    }

    bool operator != (const IChildrenIterator& other) const {
        return wrappedType_::operator != (static_cast<wrappedType_>(other));
    }

    IChildrenIterator& operator ++ () {
        wrappedType_::operator ++ ();
        return *this;
    }

    IChildrenIterator operator ++ (int) {
        return ChildrenIterator(wrappedType_::operator ++ (0));
    }

    VisitorT operator * () const {
        return VisitorT(wrappedType_::operator * ().second);
    }

    friend VisitorT;
private:
    typedef typename std::map<T, NodeT*>::const_iterator wrappedType_;

    IChildrenIterator(const wrappedType_& iter) : wrappedType_(iter) {}
};

template<typename T>
class TreeVisitor {
public:
    typedef IChildrenIterator<T, Node<T>, TreeVisitor> ChildrenIterator;

    TreeVisitor() = default;

    TreeVisitor(const TreeVisitor&) = default;
    TreeVisitor& operator = (const TreeVisitor&) = default;
    TreeVisitor(TreeVisitor&&) noexcept = default;
    TreeVisitor& operator = (TreeVisitor&&) noexcept = default;

    ~TreeVisitor() = default;

    static TreeVisitor rootVisitor(const SuffixTree<T>& tree) {
        return TreeVisitor(tree.root_);
    }

    ChildrenIterator begin() const {
        return ChildrenIterator(node_->children_.cbegin());
    }

    ChildrenIterator end() const {
        return ChildrenIterator(node_->children_.cend());
    }

    bool isLeaf() const {
        return node_->isLeaf();
    }

    size_t getLength() const {
        return node_->getLength();
    }

    ssize_t getStart() const {
        return node_->edge_start_;
    }

    ssize_t getFinish() const {
        return node_->edge_finish_;
    }

    template<typename Container>
    std::vector<T> getSubstring(const Container& values) const {
        std::vector<T> answer;
        const Node<T>* curr = node_;
        while (curr->edge_finish_ != 0) {
            answer.insert(answer.end(),
                          std::make_reverse_iterator(values.begin() + normalizeIndex(curr, values.size())),
                          std::make_reverse_iterator(values.begin() + curr->edge_start_));
            curr = curr->parent_;
        }
        std::reverse(answer.begin(), answer.end());
        return answer;
    }

    friend ChildrenIterator;
private:
    explicit TreeVisitor(const Node<T>* node) : node_(node) {}

    size_t normalizeIndex(const Node<T>* node, size_t values_num) const {
        return (node->isLeaf() ? values_num : node->edge_finish_);
    }

    const Node<T>* node_;
};

template<typename T, typename Container = std::vector<T>>
class RefrenAlgorithmTree {
public:
    explicit RefrenAlgorithmTree(const Container& values) : tree_(), values_(values), ans_(0) {}

    void execute() {
        for (const T& symb : values_) {
            tree_.add(symb);
        }
        tree_.add(T());
        auto vis = TreeVisitor<T>::rootVisitor(tree_);
        dfs_(vis);
    }

    size_t sizeRefren() const {
        return ans_;
    }

    std::vector<T> getRefren() const {
        return vis_ans_.getSubstring(values_);
    }
private:
    void updateAnswer_(const TreeVisitor<T>& vis, size_t leaves, size_t depth) {
        if (leaves * depth > ans_) {
            ans_ = leaves * depth;
            vis_ans_ = vis;
        }
    }

    size_t dfs_(const TreeVisitor<T>& vis, size_t depth = 0) {
        if (vis.isLeaf()) {
            if (vis.getStart() != static_cast<ssize_t>(values_.size()))
                updateAnswer_(vis, 1, depth + values_.size() - vis.getStart() - vis.getLength());
            return 1;
        }
        size_t cnt_leaves = 0;
        for (auto it = vis.begin(); it != vis.end(); ++it) {
            cnt_leaves += dfs_(*it, depth + (*it).getLength());
        }
        updateAnswer_(vis, cnt_leaves, depth);
        return cnt_leaves;
    }

    SuffixTree<T> tree_;
    Container values_;
    TreeVisitor<T> vis_ans_;
    size_t ans_;
};


// =======================================================================

template<typename T>
class AutomatonVisitor;

template<typename T>
class SuffixAutomaton;

template<typename T>
class State {
public:
    State() = default;

    State(State&& other) noexcept:
        transitions_(std::move(other.transitions_)), link_(other.link_), len_(other.len_),
        first_(other.first_), term_(other.term_) {
        other.transitions_ = std::map<T, State*>();
    }

    State(size_t len) : transitions_(), link_(nullptr), len_(len), first_(len - 1), term_(false) {}

    State& operator = (State&& other) noexcept {
        if (this == &other)
            return *this;
        transitions_ = std::move(other.transitions_);
        link_ = other.link_;
        len_ = other.len_;
        first_ = other.first_;
        term_ = other.term_;
        other.transitions_ = std::map<T, State*>();
        return *this;
    }

    bool canGo(const T& symb) const {
        return (transitions_.find(symb) != transitions_.end());
    }

    State* go(const T& symb) {
        return transitions_[symb];
    }

    void addTransition(const T& symb, State* state) {
        transitions_[symb] = state;
    }

    State* getLink() const {
        return link_;
    }

    void setLink(State* state) {
        link_ = state;
    }

    size_t getLen() const {
        return len_;
    }

    void setLen(size_t len) {
        len_ = len;
    }

    size_t getFirst() const {
        return first_;
    }

    size_t getId() const {
        return id_;
    }

    void setTerminal() {
        term_ = true;
    }

    bool isTerminal() const {
        return term_;
    }

    State* clone() {
        return new State(*this);
    }

    ~State() = default;

    template<typename U>
    friend class SuffixAutomaton;

    template<typename U>
    friend class AutomatonVisitor;
private:
    State(const State&) = default;
    State& operator = (const State&) = default;

    void setId(size_t id) {
        id_ = id;
    }

    std::map<T, State*> transitions_;
    State* link_;
    size_t len_, first_, id_;
    bool term_;
};

template<typename T>
class SuffixAutomaton {
public:
    SuffixAutomaton() : root_(new State<T>()), last_(root_), states_number_(1) {}

    SuffixAutomaton(const SuffixAutomaton&) = delete;
    SuffixAutomaton& operator=(const SuffixAutomaton&) = delete;

    SuffixAutomaton(SuffixAutomaton&& other) noexcept : root_(other.root_), last_(other.last_) {
        other.root_ = nullptr;
        other.last_ = nullptr;
    }

    SuffixAutomaton& operator=(SuffixAutomaton&& other) noexcept {
        if (this == &other)
            return *this;
        root_ = other.root_;
        last_ = other.last_;
        other.root_ = nullptr;
        other.last_ = nullptr;
        return *this;
    }

    ~SuffixAutomaton() {
        auto states = statesListForDeletion_();
        for (State<T>* state : states)
            delete state;
    }

    size_t numberOfStates() const {
        return states_number_;
    }

    void add(const T& symb) {
        State<T>* nlast = new State<T>(last_->getLen() + 1);
        nlast->setId(states_number_++);
        State<T>* curr = last_;
        while (curr != nullptr && !curr->canGo(symb)) {
            curr->addTransition(symb, nlast);
            curr = curr->getLink();
        }
        if (curr == nullptr) {
            nlast->setLink(root_);
            last_ = nlast;
            return;
        }
        State<T>* next = curr->go(symb);
        if (curr->getLen() + 1 == next->getLen())
            nlast->setLink(next);
        else {
            State<T>* clone = next->clone();
            clone->setId(states_number_++);
            clone->setLen(curr->getLen() + 1);
            next->setLink(clone);
            nlast->setLink(clone);
            while (curr != nullptr && curr->canGo(symb) && curr->go(symb) == next) {
                curr->addTransition(symb, clone);
                curr = curr->getLink();
            }
        }
        last_ = nlast;
    }

    template<typename Container>
    State<T>* go(const Container& values) {
        State<T>* curr = root_;
        for (const auto& val : values)
            curr = curr->go(val);
        return curr;
    }

    void setTerminal() {
        State<T>* curr = last_;
        while (curr != nullptr) {
            curr->setTerminal();
            curr = curr->getLink();
        }
    }

    template<typename U>
    friend class AutomatonVisitor;
private:
    std::vector<State<T>*> statesListForDeletion_() {
        std::vector<State<T>*> ans;
        std::queue<State<T>*> queue;
        queue.push(root_);
        while (!queue.empty()) {
            State<T>* curr = queue.front();
            queue.pop();
            ans.push_back(curr);
            for (auto child : curr->transitions_) {
                if (child.second->link_ == nullptr)
                    continue;
                child.second->link_ = nullptr;
                queue.push(child.second);
            }
        }
        return ans;
    }

    State<T>* root_, *last_;
    size_t states_number_;
};

template<typename T>
class AutomatonVisitor {
public:
    typedef IChildrenIterator<T, State<T>, AutomatonVisitor> ChildrenIterator;

    AutomatonVisitor() = default;

    AutomatonVisitor(const AutomatonVisitor&) = default;
    AutomatonVisitor& operator = (const AutomatonVisitor&) = default;
    AutomatonVisitor(AutomatonVisitor&&) noexcept = default;
    AutomatonVisitor& operator = (AutomatonVisitor&&) noexcept = default;

    ~AutomatonVisitor() = default;

    static AutomatonVisitor rootVisitor(const SuffixAutomaton<T>& tree) {
        return AutomatonVisitor(tree.root_);
    }

    ChildrenIterator begin() const {
        return ChildrenIterator(state_->transitions_.cbegin());
    }

    ChildrenIterator end() const {
        return ChildrenIterator(state_->transitions_.cend());
    }

    size_t getId() const {
        return state_->getId();
    }

    bool isTerminal() const {
        return state_->isTerminal();
    }

    size_t getLen() const {
        return state_->getLen();
    }

    size_t getFirst() const {
        return state_->first_;
    }

    friend ChildrenIterator;
private:
    AutomatonVisitor(State<T>* state) : state_(state) {}

    State<T>* state_;
};

template<typename T, typename Container = std::vector<T>>
class RefrenAlgorithmAutomaton {
public:
    explicit RefrenAlgorithmAutomaton(const Container& values) : aut_(), values_(values), ans_(0) {}

    void execute() {
        for (const T& symb : values_)
            aut_.add(symb);
        aut_.setTerminal();

        auto vis = AutomatonVisitor<T>::rootVisitor(aut_);
        term_achievable_.resize(aut_.numberOfStates());
        dfs_(vis);
    }

    size_t sizeRefren() const {
        return ans_;
    }

    std::vector<T> getRefren() const {
        size_t first = vis_ans_.getFirst(), len = vis_ans_.getLen();
        first += 1 - len;
        return std::vector<T>(values_.begin() + first, values_.begin() + first + len);
    }

private:
    void updateAnswer_(const AutomatonVisitor<T>& vis, size_t term) {
        if (vis.getLen() * term > ans_) {
            ans_ = vis.getLen() * term;
            vis_ans_ = vis;
        }
    }

    size_t dfs_(const AutomatonVisitor<T>& vis) {
        if (term_achievable_[vis.getId()] != 0)
            return term_achievable_[vis.getId()];
        size_t terminals = vis.isTerminal();
        for (auto it = vis.begin(); it != vis.end(); ++it) {
            terminals += dfs_(*it);
        }
        term_achievable_[vis.getId()] = terminals;
        updateAnswer_(vis, terminals);
        return terminals;
    }

    SuffixAutomaton<T> aut_;
    Container values_;
    AutomatonVisitor<T> vis_ans_;
    std::vector<size_t> term_achievable_;
    size_t ans_;
};

// =======================================================================


class Data {
public:
    Data() = default;

    Data(const Data&) = default;
    Data(Data&&) = default;

    Data& operator = (const Data&) = default;
    Data& operator = (Data&&) = default;

    ~Data() = default;

    const std::vector<size_t>& getValues() const {
        return values_;
    }

    friend std::istream& operator >> (std::istream&, Data&);
private:
    std::vector<size_t> values_;
};

class ResultInfo {
public:
    ResultInfo() = default;

    ResultInfo(const ResultInfo&) = default;
    ResultInfo(ResultInfo&&) = default;

    template<typename Algorithm>
    ResultInfo(const Algorithm& algo):
        refr_(algo.sizeRefren()), answer_(algo.getRefren()) {}

    ResultInfo& operator = (const ResultInfo&) = default;
    ResultInfo& operator = (ResultInfo&&) = default;

    ~ResultInfo() = default;

    friend std::ostream& operator << (std::ostream&, const ResultInfo&);
private:
    size_t refr_;
    std::vector<size_t> answer_;
};

std::istream& operator >> (std::istream& in, Data& data) {
    size_t n, m;
    in >> n >> m;
    data.values_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        in >> data.values_[i];
    }
    return in;
}

std::ostream& operator << (std::ostream& out, const ResultInfo& info) {
    out << info.refr_ << "\n" << info.answer_.size() << "\n";
    for (auto elem : info.answer_)
        out << elem << " ";
    out << "\n";
    return out;
}

template<typename Algorithm>
ResultInfo run(const Data& data) {
    Algorithm algo(data.getValues());
    algo.execute();
    return ResultInfo(algo);
}

template<typename Algorithm>
void process(std::istream& in, std::ostream& out) {
    Data data;
    in >> data;

    auto result = run<Algorithm>(data);

    out << result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);
    process<RefrenAlgorithmAutomaton<size_t>>(std::cin, std::cout);
    return 0;
}