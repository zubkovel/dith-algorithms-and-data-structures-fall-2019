#include <iostream>
#include <vector>
#include <string>
//const char min_charter

std::vector<int> CountSort(const std::string &txt, size_t size) {
    std::vector<int> count(size, 0);
    const int txt_size = txt.size();

    for (int i = 0; i < txt_size; ++i) {
        ++count[txt[i]];
    }

    for (int i = 1; i < size; ++i) {
        count[i] += count[i - 1];
    }

    std::vector<int> sort_suffs(txt_size, 0);
    for (int i = 0; i < txt_size; ++i) {
        sort_suffs[--count[txt[i]]] = i;
    }
    return sort_suffs;
}

std::vector<int> BuildSuffixArray(std::string txt) {
    static const char min_char = '$';
    static const int alphabet_size = 128;

    txt += min_char;
    const int txt_size = txt.size();
    std::vector<int> sort_suffs = CountSort(txt, alphabet_size);

    std::vector<int> count(alphabet_size, 0);
    std::vector<std::vector<int>> classes(1, std::vector<int>(txt_size));
    classes[0][sort_suffs[0]] = 0;

    int classesN = 1;
    for (int i = 1; i < txt_size; ++i) {
        if (txt[sort_suffs[i]] != txt[sort_suffs[i - 1]]) {
            ++classesN;
        }
        classes[0][sort_suffs[i]] = classesN - 1;
    }

    std::vector<int> new_suffs(txt_size);
    for (int k = 0; (1 << k) < txt_size; ++k) {
        for (int i = 0; i < txt_size; ++i) {
            new_suffs[i] = sort_suffs[i] - (1 << k);
            if (new_suffs[i] < 0) {
                new_suffs[i] += txt_size;
            }
        }

        count.assign(classesN, 0);
        for (int i = 0; i < txt_size; ++i) {
            count[classes[k][new_suffs[i]]]++;
        }

        for (int i = 1; i < classesN; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = txt_size - 1; i >= 0; --i) {
            sort_suffs[--count[classes[k][new_suffs[i]]]] = new_suffs[i];
        }

        classes.emplace_back(txt_size);
        classes[k + 1][sort_suffs[0]] = 0;
        classesN = 1;
        for (int i = 1; i < txt_size; i++) {
            const int mid1 = (sort_suffs[i] + (1 << k)) % txt_size;
            const int mid2 = (sort_suffs[i - 1] + (1 << k)) % txt_size;

            if (classes[k][sort_suffs[i]] != classes[k][sort_suffs[i - 1]]
                || classes[k][mid1] != classes[k][mid2]) {
                classesN++;
            }
            classes[k + 1][sort_suffs[i]] = classesN - 1;
        }
    }
    sort_suffs.erase(sort_suffs.begin());
    return sort_suffs;
}


std::vector<int> Kasai(const std::string &txt,
                       const std::vector<int> &suffix_array) {

    int suffix_array_size = suffix_array.size();

    std::vector<int> lcp(suffix_array_size, 0);
    std::vector<int> inv_suffs(suffix_array_size, 0);

    for (int i = 0; i < suffix_array_size; i++) {
        inv_suffs[suffix_array[i]] = i;
    }

    int k = 0;
    for (int i = 0; i < suffix_array_size; i++) {
        if (inv_suffs[i] == suffix_array_size - 1) {
            k = 0;
            continue;
        }

        int j = suffix_array[inv_suffs[i] + 1];
        while (std::max(i, j) + k < suffix_array_size
               && txt[i + k] == txt[j + k]) {
            ++k;
        }
        lcp[inv_suffs[i]] = k;
        k = (k > 0) ? k - 1 : k;
    }
    return lcp;
}

int CountDistinctSubstring(const std::string &txt) {
    int txt_size = txt.size();

    const auto suffix_array = BuildSuffixArray(txt);
    const auto lcp = Kasai(txt, suffix_array);

    int result = txt_size - suffix_array[0];

    for (int i = 1; i < lcp.size(); i++) {
        result += (txt_size - suffix_array[i]) - lcp[i - 1];
    }
    return result;
}

int main() {
    std::string txt;
    std::cin >> txt;
    std::cout << CountDistinctSubstring(std::move(txt));
    return 0;
}