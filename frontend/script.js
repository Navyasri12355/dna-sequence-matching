const API_URL = "http://127.0.0.1:5000/api";

function runMatch() {
    const text = document.getElementById("text").value.trim();
    const pattern = document.getElementById("pattern").value.trim();
    const algorithm = document.getElementById("algorithm").value;

    if (!text || !pattern) {
        alert("Please enter both text and pattern.");
        return;
    }

    fetch(`${API_URL}/match`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ text, pattern, algorithm })
    })
    .then(res => res.json())
    .then(showResult)
    .catch(err => {
        console.error(err);
        alert("Error occurred while running the algorithm.");
    });
}

function compareAll() {
    const text = document.getElementById("text").value.trim();
    const pattern = document.getElementById("pattern").value.trim();

    if (!text || !pattern) {
        alert("Please enter both text and pattern.");
        return;
    }

    fetch(`${API_URL}/compare`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ text, pattern })
    })
    .then(res => res.json())
    .then(response => {
        const results = response.results || [];
        const fastest = response.fastest || null;

        if (!results.length) {
            document.getElementById("result").innerHTML = "<p>No results returned.</p>";
            return;
        }

        let html = "<h3>Algorithm Comparison:</h3><ul>";
        results.forEach(r => {
            const isFastest = r.algorithm === fastest;
            html += `<li style="margin-bottom: 12px;">
                        <strong style="color: ${isFastest ? '#27ae60' : '#2980b9'}">
                            ${r.algorithm}${isFastest ? "(Fastest)" : ""}
                        </strong><br>
                        Matches: ${r.matches ?? 0}<br>
                        Indices: ${r.indices?.join(", ") || "None"}<br>
                        Time Taken: ${r.time_ms?.toFixed(6) || "N/A"} ms<br>
                        Best Case: ${r.best || "N/A"}<br>
                        Worst Case: ${r.worst || "N/A"}<br>
                        Average Case: ${r.avg || "N/A"}
                     </li>`;
        });
        html += "</ul>";
        document.getElementById("result").innerHTML = html;
    })
    .catch(err => {
        console.error(err);
        alert("Error occurred while comparing algorithms.");
    });
}

function showResult(data) {
    if (data.error) {
        alert("Error: " + data.error);
        return;
    }

    const html = `<h3>${data.algorithm} Result</h3>
                  Matches: ${data.matches}<br>
                  Indices: ${data.indices?.join(", ") || "None"}<br>
                  Time Taken: ${data.time_ms?.toFixed(6) || "N/A"} ms<br>
                  Best Case: ${data.best_case || "N/A"}<br>
                  Worst Case: ${data.worst_case || "N/A"}<br>
                  Average Case: ${data.average_case || "N/A"}`;
    document.getElementById("result").innerHTML = html;
}
