#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

class Package {
public:
    std::string name;
    std::string version;

    Package(const std::string& name, const std::string& version)
        : name(name), version(version) {
    }

    bool operator==(const Package& other) const {
        return name == other.name && version == other.version;
    }

    struct Hash {
        size_t operator()(const Package& p) const {
            return std::hash<std::string>{}(p.name) ^ std::hash<std::string>{}(p.version);
        }
    };
};

class DependencyGraph {
public:
    std::unordered_map<Package, std::unordered_set<Package, Package::Hash>, Package::Hash> graph;
    std::unordered_map<Package, int, Package::Hash> inDegree;

    void addPackage(const Package& pkg) {
        if (graph.find(pkg) == graph.end()) {
            graph[pkg] = std::unordered_set<Package, Package::Hash>();
        }
        inDegree[pkg] = 0;
    }

    void addDependency(const Package& pkg, const Package& dependency) {
        graph[pkg].insert(dependency);
        inDegree[dependency]++;
    }

    const std::unordered_set<Package, Package::Hash>& getDependencies(const Package& pkg) const {
        return graph.at(pkg);
    }

    int getInDegree(const Package& pkg) const {
        return inDegree.at(pkg);
    }

    void setInDegree(const Package& pkg, int degree) {
        inDegree[pkg] = degree;
    }

    bool packageExists(const Package& pkg) const {
        return graph.find(pkg) != graph.end();
    }
};

class CycleDetector {
public:
    bool detectCycle(DependencyGraph& graph) {
        std::unordered_set<Package, Package::Hash> visited;
        std::unordered_set<Package, Package::Hash> recStack;

        for (const auto& pair : graph.graph) {
            if (detectCycleUtil(pair.first, graph, visited, recStack)) {
                return true;
            }
        }
        return false;
    }

private:
    bool detectCycleUtil(const Package& pkg, DependencyGraph& graph,
        std::unordered_set<Package, Package::Hash>& visited,
        std::unordered_set<Package, Package::Hash>& recStack) {
        if (recStack.find(pkg) != recStack.end()) {
            return true;
        }

        if (visited.find(pkg) != visited.end()) {
            return false;
        }

        visited.insert(pkg);
        recStack.insert(pkg);

        for (const auto& dep : graph.getDependencies(pkg)) {
            if (detectCycleUtil(dep, graph, visited, recStack)) {
                return true;
            }
        }

        recStack.erase(pkg);
        return false;
    }
};

class TopologicalSorter {
public:
    void topologicalSort(DependencyGraph& graph) {
        std::queue<Package> zeroInDegreeQueue;
        std::vector<Package> sortedPackages;

        for (const auto& pair : graph.graph) {
            if (graph.getInDegree(pair.first) == 0) {
                zeroInDegreeQueue.push(pair.first);
            }
        }

        while (!zeroInDegreeQueue.empty()) {
            Package pkg = zeroInDegreeQueue.front();
            zeroInDegreeQueue.pop();
            sortedPackages.push_back(pkg);

            for (const auto& dep : graph.getDependencies(pkg)) {
                graph.setInDegree(dep, graph.getInDegree(dep) - 1);
                if (graph.getInDegree(dep) == 0) {
                    zeroInDegreeQueue.push(dep);
                }
            }
        }

        if (sortedPackages.size() != graph.graph.size()) {
            std::cout << "Cycle detected! Topological sort is not possible.\n";
        }
        else {
            std::cout << "Topological Order (Build Order):\n";
            for (const auto& pkg : sortedPackages) {
                std::cout << pkg.name << " " << pkg.version << "\n";
            }
        }
    }
};

class VersionResolver {
public:
    static bool resolveVersion(const Package& pkg1, const Package& pkg2) {
        return pkg1.version == pkg2.version;
    }
};

class ConflictDetector {
public:
    bool detectConflict(DependencyGraph& graph) {
        for (const auto& pair : graph.graph) {
            const Package& pkg = pair.first;
            for (const auto& dep : pair.second) {
                if (!VersionResolver::resolveVersion(pkg, dep)) {
                    std::cout << "Conflict detected: " << pkg.name << " and " << dep.name << " have incompatible versions.\n";
                    return true;
                }
            }
        }
        return false;
    }
};

class Visualizer {
public:
    void visualize(const DependencyGraph& graph) {
        std::cout << "digraph dependencies {\n";
        for (const auto& pair : graph.graph) {
            for (const auto& dep : pair.second) {
                std::cout << "  \"" << pair.first.name << " " << pair.first.version
                    << "\" -> \"" << dep.name << " " << dep.version << "\";\n";
            }
        }
        std::cout << "}\n";
    }
};

class ImpactAnalyzer {
public:
    void analyzeImpact(DependencyGraph& graph, const Package& pkg) {
        std::cout << "Impact analysis for package: " << pkg.name << "\n";
        for (const auto& pair : graph.graph) {
            if (pair.second.find(pkg) != pair.second.end()) {
                std::cout << "Package " << pair.first.name << " depends on " << pkg.name << "\n";
            }
        }
    }
};

int main() {
    DependencyGraph graph;

    Package pkgA("pkgA", "1.0.1");
    Package pkgB("pkgB", "2.3.0");
    Package pkgC("pkgC", "3.1.2");
    Package pkgD("pkgD", "1.5.0");

    graph.addPackage(pkgA);
    graph.addPackage(pkgB);
    graph.addPackage(pkgC);
    graph.addPackage(pkgD);

    graph.addDependency(pkgA, pkgB);
    graph.addDependency(pkgB, pkgC);
    graph.addDependency(pkgA, pkgD);

    CycleDetector cycleDetector;
    if (cycleDetector.detectCycle(graph)) {
        std::cout << "Cycle detected! Resolving is not possible.\n";
        return 1;
    }

    TopologicalSorter sorter;
    sorter.topologicalSort(graph);

    ConflictDetector conflictDetector;
    if (conflictDetector.detectConflict(graph)) {
        std::cout << "Conflict detected!\n";
    }

    Visualizer visualizer;
    visualizer.visualize(graph);

    ImpactAnalyzer impactAnalyzer;
    impactAnalyzer.analyzeImpact(graph, pkgB);

    return 0;
}
