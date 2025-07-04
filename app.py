from flask import Flask, request, jsonify
from flask_cors import CORS
from time import perf_counter
import importlib

app = Flask(__name__)
CORS(app)

# Load compiled C extension (from stringmatch.c built with pybind11)
stringmatch = importlib.import_module("stringmatch")

# Function mapping (use readable names matching frontend <select>)
algorithms = {
    "Naive": stringmatch.naive_search,
    "KMP": stringmatch.kmp_search,
    "Rabin-Karp": stringmatch.rabin_karp,
    "Boyer-Moore": stringmatch.boyer_moore,
    "Aho-Corasick": stringmatch.aho_corasick,
    "Finite Automata": stringmatch.finite_automata  # 🔁 FIXED name
}

@app.route("/api/match", methods=["POST"])
def match_single():
    if request.method != "POST":
        return jsonify({"error": "Only POST allowed"}), 405

    data = request.get_json()
    text = data.get("text", "").strip()
    pattern = data.get("pattern", "").strip()
    method = data.get("algorithm", "").strip()

    if method not in algorithms:
        return jsonify({"error": f"Unknown algorithm '{method}'."}), 400

    try:
        start = perf_counter()
        count, indices, best, worst, avg = algorithms[method](text, pattern)
        elapsed = (perf_counter() - start) * 1000  # ms

        return jsonify({
            "algorithm": method,
            "matches": count,
            "indices": indices,
            "best_case": best,
            "worst_case": worst,
            "average_case": avg,
            "time_ms": round(elapsed, 6)
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route("/api/compare", methods=["POST"])
def compare_all():
    data = request.get_json()
    text = data.get("text", "").strip()
    pattern = data.get("pattern", "").strip()

    results = []
    fastest_time = float("inf")
    fastest_algo = None

    for algo_name, func in algorithms.items():
        try:
            start = perf_counter()
            count, indices, best, worst, avg = func(text, pattern)
            elapsed = (perf_counter() - start) * 1000  # ms

            if elapsed < fastest_time:
                fastest_time = elapsed
                fastest_algo = algo_name

            results.append({
                "algorithm": algo_name,
                "matches": count,
                "indices": indices,
                "time_ms": round(elapsed, 6),
                "best": best,
                "worst": worst,
                "avg": avg
            })

        except Exception as e:
            results.append({
                "algorithm": algo_name,
                "error": str(e)
            })

    return jsonify({
        "results": results,
        "fastest": fastest_algo
    })

if __name__ == "__main__":
    app.run(debug=True)
