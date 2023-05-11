#pragma once

#include <vector>

class ScapegoatTree
{
    class Node
    {
        friend class ScapegoatTree;
        int m_value;
        std::size_t m_size = 0;
        Node * m_left = nullptr;
        Node * m_right = nullptr;
        Node * m_parent = nullptr;

        Node(int value);
        ~Node();

        std::size_t size() const;
        bool empty() const;
        void recursiveTraversal(const Node * node, std::vector<int> & vec) const;
        std::vector<int> values() const;
    };

    Node * m_root = nullptr;
    const double m_alpha = 0.8; // The most optimal value for typical tree

    Node * findElementOrAncestor(Node * node, const int & value) const;
    static void checkAlpha(const double & alpha);
    void updateSize(Node * node, int delta);
    static void replaceWithRightNode(Node * node);
    static void removeImpl(Node * node);
    bool unbalancedDescendants(Node * node) const;
    static void buildBalanced(Node * tree);
    static Node * buildRecursive(const std::vector<int> & elements, int l, int r);
    static void deleteIntermediate(Node * target, Node * tree);
    static void deleteLeftDescendant(Node * tree);
    static void deleteRightDescendant(Node * tree);
    static void createLeftDescendant(Node * node, const int & value);
    static void createRightDescendant(Node * node, const int & value);

public:
    ScapegoatTree() = default;
    ScapegoatTree(double alpha);

    bool contains(int value) const;
    bool insert(int value);
    bool remove(int value);

    std::size_t size() const;
    bool empty() const;

    std::vector<int> values() const;

    ~ScapegoatTree();
};
