#include <Python.h>
#include <string.h>
#include <stdlib.h>

void append_index(PyObject* list, int index) {
    PyObject* val = PyLong_FromLong(index);
    PyList_Append(list, val);
    Py_DECREF(val);
}

//Naive
static PyObject* naive_search(PyObject* self, PyObject* args) {
    const char *text, *pattern;
    if (!PyArg_ParseTuple(args, "ss", &text, &pattern)) return NULL;
    int n = strlen(text), m = strlen(pattern), count = 0;
    PyObject* indices = PyList_New(0);
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++)
            if (text[i + j] != pattern[j]) break;
        if (j == m) { append_index(indices, i); count++; }
    }
    return Py_BuildValue("iOsss", count, indices, "O(n)", "O(nm)", "O(nm)");
}

//KMP
void compute_lps(const char *pat, int M, int *lps) {
    int len = 0; lps[0] = 0;
    for (int i = 1; i < M;) {
        if (pat[i] == pat[len]) lps[i++] = ++len;
        else if (len) len = lps[len - 1];
        else lps[i++] = 0;
    }
}
static PyObject* kmp_search(PyObject* self, PyObject* args) {
    const char *text, *pattern;
    if (!PyArg_ParseTuple(args, "ss", &text, &pattern)) return NULL;
    int N = strlen(text), M = strlen(pattern), i = 0, j = 0, count = 0;
    int *lps = (int*)malloc(sizeof(int) * M);
    compute_lps(pattern, M, lps);
    PyObject* indices = PyList_New(0);
    while (i < N) {
        if (pattern[j] == text[i]) { i++; j++; }
        if (j == M) { append_index(indices, i - j); count++; j = lps[j - 1]; }
        else if (i < N && pattern[j] != text[i])
            j ? (j = lps[j - 1]) : i++;
    }
    free(lps);
    return Py_BuildValue("iOsss", count, indices, "O(n+m)", "O(n+m)", "O(n+m)");
}

//Rabin-Karp
static PyObject* rabin_karp(PyObject* self, PyObject* args) {
    const char *text, *pattern;
    if (!PyArg_ParseTuple(args, "ss", &text, &pattern)) return NULL;
    int d = 256, q = 101;
    int M = strlen(pattern), N = strlen(text), p = 0, t = 0, h = 1, i, j, count = 0;
    PyObject* indices = PyList_New(0);
    for (i = 0; i < M - 1; i++) h = (h * d) % q;
    for (i = 0; i < M; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }
    for (i = 0; i <= N - M; i++) {
        if (p == t) {
            for (j = 0; j < M; j++)
                if (text[i + j] != pattern[j]) break;
            if (j == M) { append_index(indices, i); count++; }
        }
        if (i < N - M) {
            t = (d * (t - text[i] * h) + text[i + M]) % q;
            if (t < 0) t += q;
        }
    }
    return Py_BuildValue("iOsss", count, indices, "O(n+m)", "O(n*m)", "O(n+m)");
}

//Boyer-Moore
void badCharHeuristic(const char *str, int size, int badchar[256]) {
    for (int i = 0; i < 256; i++) badchar[i] = -1;
    for (int i = 0; i < size; i++) badchar[(int) str[i]] = i;
}
static PyObject* boyer_moore(PyObject* self, PyObject* args) {
    const char *text, *pattern;
    if (!PyArg_ParseTuple(args, "ss", &text, &pattern)) return NULL;
    int m = strlen(pattern), n = strlen(text), count = 0;
    int badchar[256];
    PyObject* indices = PyList_New(0);
    badCharHeuristic(pattern, m, badchar);
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) j--;
        if (j < 0) {
            append_index(indices, s); count++;
            s += (s + m < n) ? m - badchar[text[s + m]] : 1;
        } else {
            s += ((j - badchar[text[s + j]]) > 1) ? (j - badchar[text[s + j]]) : 1;
        }
    }
    return Py_BuildValue("iOsss", count, indices, "O(n/m)", "O(n*m)", "O(n)");
}

//Aho-Corasick (Single Pattern simplified)
typedef struct TrieNode {
    struct TrieNode *children[256];
    struct TrieNode *fail;
    int is_end;
} TrieNode;

TrieNode *create_node() {
    TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
    return node;
}
void insert(TrieNode *root, const char *pattern) {
    TrieNode *curr = root;
    for (int i = 0; pattern[i]; i++) {
        unsigned char c = pattern[i];
        if (!curr->children[c]) curr->children[c] = create_node();
        curr = curr->children[c];
    }
    curr->is_end = 1;
}
void build_failure(TrieNode *root) {
    TrieNode *queue[1000]; int front = 0, rear = 0;
    root->fail = root; queue[rear++] = root;
    while (front < rear) {
        TrieNode *curr = queue[front++];
        for (int c = 0; c < 256; c++) {
            TrieNode *child = curr->children[c];
            if (!child) continue;
            TrieNode *f = curr->fail;
            while (f != root && !f->children[c]) f = f->fail;
            child->fail = (f->children[c] && f->children[c] != child) ? f->children[c] : root;
            queue[rear++] = child;
        }
    }
}
static PyObject* aho_corasick(PyObject* self, PyObject* args) {
    const char *text, *pattern;
    if (!PyArg_ParseTuple(args, "ss", &text, &pattern)) return NULL;
    TrieNode *root = create_node();
    insert(root, pattern);
    build_failure(root);
    TrieNode *curr = root;
    PyObject *indices = PyList_New(0);
    int count = 0;
    for (int i = 0; text[i]; i++) {
        unsigned char c = text[i];
        while (curr != root && !curr->children[c]) curr = curr->fail;
        if (curr->children[c]) curr = curr->children[c];
        if (curr->is_end) { append_index(indices, i - strlen(pattern) + 1); count++; }
    }
    return Py_BuildValue("iOsss", count, indices, "O(n + m + z)", "O(n + m + z)", "O(n + m + z)");
}

//Finite Automata
int getNextState(const char *pat, int M, int state, int x) {
    if (state < M && x == pat[state]) return state + 1;
    for (int ns = state; ns > 0; ns--) {
        if (pat[ns - 1] == x) {
            int i = 0;
            for (; i < ns - 1; i++)
                if (pat[i] != pat[state - ns + 1 + i]) break;
            if (i == ns - 1) return ns;
        }
    }
    return 0;
}
void computeTF(const char *pat, int M, int TF[][256]) {
    for (int state = 0; state <= M; ++state)
        for (int x = 0; x < 256; ++x)
            TF[state][x] = getNextState(pat, M, state, x);
}
static PyObject* finite_automata(PyObject* self, PyObject* args) {
    const char *txt, *pat;
    if (!PyArg_ParseTuple(args, "ss", &txt, &pat)) return NULL;
    int M = strlen(pat), N = strlen(txt), TF[100][256], state = 0, count = 0;
    computeTF(pat, M, TF);
    PyObject *indices = PyList_New(0);
    for (int i = 0; i < N; i++) {
        state = TF[state][(int)txt[i]];
        if (state == M) { append_index(indices, i - M + 1); count++; }
    }
    return Py_BuildValue("iOsss", count, indices, "O(mk+n)", "O(mk+n)", "O(mk+n)");
}

static PyMethodDef StringMethods[] = {
    {"naive_search", naive_search, METH_VARARGS, ""},
    {"kmp_search", kmp_search, METH_VARARGS, ""},
    {"rabin_karp", rabin_karp, METH_VARARGS, ""},
    {"boyer_moore", boyer_moore, METH_VARARGS, ""},
    {"aho_corasick", aho_corasick, METH_VARARGS, ""},
    {"finite_automata", finite_automata, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef stringmodule = {
    PyModuleDef_HEAD_INIT, "stringmatch", NULL, -1, StringMethods
};

PyMODINIT_FUNC PyInit_stringmatch(void) {
    return PyModule_Create(&stringmodule);
}
