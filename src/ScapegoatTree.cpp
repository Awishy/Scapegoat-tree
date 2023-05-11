#include "ScapegoatTree.h"

#include <stdexcept> // for std::invalid_argument exception

ScapegoatTree::ScapegoatTree(double alpha)
    : m_alpha(alpha)
{
    checkAlpha(alpha);
}

ScapegoatTree::Node::Node(int value)
    : m_value(value)
    , m_size(1)
{
}

void ScapegoatTree::checkAlpha(const double & alpha)
{
    if (alpha < 0.5 || alpha > 1.0) {
        throw std::invalid_argument("Invalid alpha: " + std::to_string(alpha));
    }
}

std::size_t ScapegoatTree::Node::size() const
{
    return m_size;
}

bool ScapegoatTree::Node::empty() const
{
    return m_size == 0;
}

ScapegoatTree::Node * ScapegoatTree::findElementOrAncestor(Node * node, const int & value) const
{
    Node * current = node;
    while (current != nullptr) {
        if (current->m_value == value) {
            return current;
        }
        if (current->m_value < value) {
            if (current->m_right == nullptr) {
                return current;
            }
            current = current->m_right;
        }
        else {
            if (current->m_left == nullptr) {
                return current;
            }
            current = current->m_left;
        }
    }
    return node;
}

bool ScapegoatTree::contains(int value) const
{
    if (empty()) {
        return false;
    }
    return findElementOrAncestor(m_root, value)->m_value == value;
}

ScapegoatTree::Node * ScapegoatTree::buildRecursive(const std::vector<int> & elements, int l, int r)
{
    if (l > r) {
        return nullptr;
    }
    int mid = (l + r) / 2;
    Node * cur = new Node(elements[mid]);
    cur->m_left = buildRecursive(elements, l, mid - 1);
    if (cur->m_left != nullptr) {
        cur->m_left->m_parent = cur;
        cur->m_size += cur->m_left->m_size;
    }
    cur->m_right = buildRecursive(elements, mid + 1, r);
    if (cur->m_right != nullptr) {
        cur->m_right->m_parent = cur;
        cur->m_size += cur->m_right->m_size;
    }
    return cur;
}

void ScapegoatTree::deleteIntermediate(Node * target, Node * tree)
{
    target->m_value = tree->m_value;
    target->m_left = tree->m_left;
    if (tree->m_left != nullptr) {
        tree->m_left->m_parent = target;
    }
    target->m_right = tree->m_right;
    if (tree->m_right != nullptr) {
        tree->m_right->m_parent = target;
    }
    tree->m_left = tree->m_right = nullptr;
    delete tree;
}

void ScapegoatTree::deleteLeftDescendant(Node * node)
{
    delete node->m_left;
    node->m_left = nullptr;
}

void ScapegoatTree::deleteRightDescendant(Node * node)
{
    delete node->m_right;
    node->m_right = nullptr;
}

void ScapegoatTree::buildBalanced(Node * node)
{
    std::vector<int> elements = node->values();
    auto temp = buildRecursive(elements, 0, elements.size() - 1);
    deleteLeftDescendant(node);
    deleteRightDescendant(node);
    deleteIntermediate(node, temp);
}

bool ScapegoatTree::unbalancedDescendants(Node * node) const
{
    return (node->m_left != nullptr && node->m_left->size() > node->size() * m_alpha) ||
            (node->m_right != nullptr && node->m_right->size() > node->size() * m_alpha);
}

void ScapegoatTree::updateSize(Node * node, int delta)
{
    node->m_size += delta;
    node = node->m_parent;
    Node * unbalanced = nullptr;
    while (node != nullptr) {
        node->m_size = 1;
        if (node->m_left != nullptr) {
            node->m_size += node->m_left->size();
            if (node->m_left->empty()) {
                deleteLeftDescendant(node);
            }
        }
        if (node->m_right != nullptr) {
            node->m_size += node->m_right->size();
            if (node->m_right->empty()) {
                deleteRightDescendant(node);
            }
        }
        if (unbalancedDescendants(node)) {
            unbalanced = node;
        }
        node = node->m_parent;
    }
    if (unbalanced != nullptr) {
        buildBalanced(unbalanced);
    }
}

void ScapegoatTree::createLeftDescendant(Node * node, const int & value)
{
    node->m_left = new Node(value);
    node->m_left->m_parent = node;
}

void ScapegoatTree::createRightDescendant(Node * node, const int & value)
{
    node->m_right = new Node(value);
    node->m_right->m_parent = node;
}

bool ScapegoatTree::insert(int value)
{
    if (empty()) {
        m_root = new Node(value);
        return true;
    }
    auto temp = findElementOrAncestor(m_root, value);
    if (temp->m_value == value) {
        return false;
    }
    if (temp->m_value < value) {
        createRightDescendant(temp, value);
    }
    else {
        createLeftDescendant(temp, value);
    }
    updateSize(temp, 1);
    return true;
}

void ScapegoatTree::replaceWithRightNode(Node * node)
{
    if (node->m_right != nullptr) {
        node->m_right->m_parent = node->m_parent;
    }
    node->m_right = nullptr;
    delete node;
}

void ScapegoatTree::removeImpl(Node * node)
{
    if (node->size() == 1) {
        return;
    }
    if (node->m_left != nullptr && node->m_right != nullptr) {
        Node * leftmost = node->m_right;
        while (leftmost->m_left != nullptr) {
            --leftmost->m_size;
            leftmost = leftmost->m_left;
        }
        node->m_value = leftmost->m_value;
        if (leftmost == node->m_right) {
            if (leftmost->m_right != nullptr) {
                node->m_right = leftmost->m_right;
                replaceWithRightNode(leftmost);
            }
            else {
                deleteRightDescendant(node);
            }
        }
        else {
            if (leftmost->m_right != nullptr) {
                leftmost->m_parent->m_left = leftmost->m_right;
                replaceWithRightNode(leftmost);
            }
            else {
                deleteLeftDescendant(leftmost->m_parent);
            }
        }
    }
    else {
        node->m_right != nullptr ? deleteIntermediate(node, node->m_right) : deleteIntermediate(node, node->m_left);
    }
}

bool ScapegoatTree::remove(int value)
{
    if (empty()) {
        return false;
    }
    auto temp = findElementOrAncestor(m_root, value);
    if (temp->m_value != value) {
        return false;
    }
    removeImpl(temp);
    updateSize(temp, -1);
    return true;
}

std::size_t ScapegoatTree::size() const
{
    if (m_root == nullptr) {
        return 0;
    }
    return m_root->size();
}

bool ScapegoatTree::empty() const
{
    return size() == 0;
}

void ScapegoatTree::Node::recursiveTraversal(const Node * node, std::vector<int> & vec) const
{
    if (node == nullptr || node->empty()) {
        return;
    }
    recursiveTraversal(node->m_left, vec);
    vec.push_back(node->m_value);
    recursiveTraversal(node->m_right, vec);
}

std::vector<int> ScapegoatTree::Node::values() const
{
    std::vector<int> result;
    recursiveTraversal(this, result);
    return result;
}

std::vector<int> ScapegoatTree::values() const
{
    if (m_root == nullptr) {
        return {};
    }
    return m_root->values();
}

ScapegoatTree::~ScapegoatTree()
{
    delete m_root;
}

ScapegoatTree::Node::~Node()
{
    delete m_left;
    delete m_right;
}
