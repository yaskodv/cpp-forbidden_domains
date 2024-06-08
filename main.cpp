#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    explicit Domain(string name) {
        assert(name.length() > 0);
        reversed_name_ = ReverseName(move(name));
    }

    string GetReversedName() {
        return reversed_name_;
    }

    bool operator==(const Domain& other) const {
        return reversed_name_ == other.reversed_name_;
    }

    bool operator!=(const Domain& other) const {
        return !(*this == other);
    }

    bool IsSubDomain(const Domain& other) const {
        return reversed_name_.starts_with(other.reversed_name_);
    }

    bool operator<(const Domain& other) const {
        return lexicographical_compare(reversed_name_.begin(), reversed_name_.end(), other.reversed_name_.begin(), other.reversed_name_.end());
    }

private:
    string reversed_name_;

    string ReverseName(string&& name) {
        //если последний символ ='.', то удаляем его
        if (name.ends_with('.')) {
            name.pop_back();
        }
        size_t last_dot_pos = name.find_last_of('.');
        if (last_dot_pos == name.npos) {
            name.push_back('.');
            return move(name);
        } else {
            string reverse_name;
            reverse_name = move(name.substr(last_dot_pos + 1));
            int dot_pos = last_dot_pos - 1;
            while (dot_pos >= 0) {
                if (name[dot_pos] == '.') {
                    reverse_name.push_back('.');
                    reverse_name += move(name.substr(dot_pos + 1, last_dot_pos - dot_pos - 1));
                    last_dot_pos = dot_pos;
                } else if (dot_pos == 0) {
                    reverse_name.push_back('.');
                    reverse_name += move(name.substr(0, last_dot_pos));
                }
                --dot_pos;
            }
            reverse_name.push_back('.');
            return reverse_name;
        }
    }
};

class DomainChecker {
public:

    template <typename It>
    DomainChecker(It first, It last) {
        for (It it = first; it != last; ++it) {
            forbidden_domains_.emplace_back(*it);
        }
        sort(forbidden_domains_.begin(),forbidden_domains_.end());

        auto sub_domain_comp = [](const auto& domain1, const auto& domain2){return domain2.IsSubDomain(domain1);};
        auto last_unique = unique(forbidden_domains_.begin(), forbidden_domains_.end(), sub_domain_comp);
        forbidden_domains_.erase(last_unique, forbidden_domains_.end());
    }

    bool IsForbidden(const Domain& domain) {
        auto next_domain_it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain);
        if (next_domain_it != forbidden_domains_.begin() && domain.IsSubDomain(*(next_domain_it - 1))) {
            return true;
        }
        return false;
    }

private:
    vector<Domain> forbidden_domains_;
};

vector<Domain> ReadDomains(istream& input, size_t num) {
    string domain_name;
    vector<Domain> domains;
    for (size_t i = 0; i < num; ++i) {
        getline(input, domain_name);
        domains.emplace_back(Domain(domain_name));
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestReverseName() {
    Domain domain1("asda.asda.ds"s);
    Domain domain2("asda.asda.ds."s);
    Domain domain3(".asda.asda.ds"s);
    Domain domain4("jhfds"s);
    Domain domain5(".jhfds."s);
    Domain domain6("jhfds."s);
    Domain domain7(".jhfds"s);
    assert(domain1.GetReversedName() == "ds.asda.asda."s);
    assert(domain2.GetReversedName() == "ds.asda.asda."s);
    assert(domain3.GetReversedName() == "ds.asda.asda."s);
    assert(domain4.GetReversedName() == "jhfds."s);
    assert(domain5.GetReversedName() == "jhfds."s);
    assert(domain6.GetReversedName() == "jhfds."s);
    assert(domain7.GetReversedName() == "jhfds."s);
}

void TestEqual() {
    Domain domain1("asda.asda.ds"s);
    Domain domain2("asda.asda.ds."s);
    Domain domain3(".asda.asda.ds"s);
    Domain domain4("jhfds"s);
    Domain domain5(".jhfds."s);
    Domain domain6("jhfds."s);
    Domain domain7(".jhfds"s);
    assert(domain1 == domain1);
    assert(domain1 == domain2);
    assert(domain1 != domain4);
}

void TestSubDomains() {
    Domain domain1("asda.asda.ds"s);
    Domain domain2("asda.ds"s);
    Domain domain3("ds"s);
    Domain domain4("jhfds.dasds"s);
    assert(domain2.IsSubDomain(domain3));
    assert(domain1.IsSubDomain(domain2));
    assert(domain1.IsSubDomain(domain3));
    assert(!domain4.IsSubDomain(domain3));
}

void TestBefore() {
    Domain domain1("asda.asda.ds"s);
    Domain domain2("asda.ds"s);
    Domain domain3("ds"s);
    Domain domain4("jhfds.dasds"s);
    assert(domain2 < domain1);
    assert(domain3 < domain2);
    assert(domain3 < domain1);
    assert(domain4 < domain3);
}

void TestForbidden() {
    Domain domain1("asda.asda.ds"s);
    Domain domain2("asda.ds"s);
    Domain domain3("as.ds"s);
    Domain domain4("ds"s);
    Domain domain5("jhfds.dasds"s);
    Domain domain6("dasds"s);
    const std::vector<Domain> forbidden_domains = {domain2, domain6};
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(checker.IsForbidden(domain1));
    assert(checker.IsForbidden(domain2));
    assert(!checker.IsForbidden(domain3));
    assert(!checker.IsForbidden(domain4));
    assert(checker.IsForbidden(domain5));
    assert(checker.IsForbidden(domain6));
}

int main() {

    TestReverseName();
    TestEqual();
    TestBefore();
    TestSubDomains();
    TestForbidden();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
