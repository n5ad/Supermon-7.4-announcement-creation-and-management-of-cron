<?php
// allmon-announcement.inc
// Include this file in Allmon3 after successful login
if (!isset($_SESSION['loggedin']) || $_SESSION['loggedin'] !== true) {
    return;
}
?>
<hr style="margin-top:30px;">
<div id="bottom_cron_section" style="margin-top:20px;">

<h3>Generate Announcement with Piper TTS</h3>
<textarea id="tts_text" rows="4" cols="60" placeholder="Enter the text you want to convert to speech..."></textarea><br><br>
<label for="tts_filename">Filename (one word, letters/numbers/hyphen/underscore only, no extension):</label><br>
<input type="text" id="tts_filename" placeholder="e.g. morningnet" style="width:220px;"><br><br>
<button id="generate_tts" class="submit">Generate WAV File</button>
<div id="tts_status" style="margin-top:10px; font-weight:bold; color:#28a745;"></div>

<h3>Announcement Scheduler</h3>

<select id="mp3_select" class="submit" style="border:2px solid #444; width: 280px;">
    <option value="">-- Select Audio File (MP3 or WAV) --</option>
</select>
<input type="button" class="submit" value="Install Announcement" id="install_announcement">
<input type="button" class="submit" value="Delete MP3/WAV" id="delete_mp3" style="background:#dc3545;color:white;border:1px solid #c82333;">
<input type="button" class="submit" value="Local Play" id="local_play_mp3" style="background:#28a745;color:white;border:1px solid #1e7e34;">
<input type="button" class="submit" value="Global Play" id="global_play_mp3" style="background:#fd7e14;color:white;border:1px solid #e06b00;">
<br><br>

<select id="ul_select" class="submit" style="border:2px solid #444; width: 280px;">
    <option value="">-- Select .ul Announcement --</option>
</select>
<input type="button" class="submit" value="Play Now" id="run_ul">
<input type="button" class="submit" value="Delete .ul" id="delete_ul" style="background:#dc3545;color:white;border:1px solid #c82333;">
<br><br>

<h3>Cron Manager</h3>
<table id="cron_table" border="1" cellpadding="5" style="width:100%; border-collapse:collapse; font-size:0.95em;">
    <thead style="background:#444;color:white;">
        <tr>
            <th>Min</th><th>Hour</th><th>DOM</th><th>Mon</th><th>DOW</th>
            <th>File</th><th>Description</th><th>Enabled</th><th>Actions</th>
        </tr>
    </thead>
    <tbody></tbody>
</table>

</div>

<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script>
$(document).ready(function() {

    function loadMP3Dropdown() {
        $.getJSON("/supermon/custom/list_mp3.php", function(files) {
            $("#mp3_select").empty().append($("<option>").val("").text("-- Select Audio File (MP3/WAV) --"));
            $.each(files, function(i, f) { $("#mp3_select").append($("<option>").val(f).text(f)); });
        }).fail(function() { console.error("Failed to load MP3 list"); });
    }
    loadMP3Dropdown();

    function loadULDropdown() {
        $.getJSON("/supermon/custom/list_ul.php", function(files) {
            $("#ul_select").empty().append($("<option>").val("").text("-- Select .ul Announcement --"));
            $.each(files, function(i, f) { $("#ul_select").append($("<option>").val(f).text(f)); });
        }).fail(function() { console.error("Failed to load UL list"); });
    }
    loadULDropdown();

    function loadCronList() {
        $.getJSON("/supermon/custom/list_cron.php", function(crons) {
            let tbody = $("#cron_table tbody").empty();
            $.each(crons, function(i, c) {
                let parts = c.time.split(/\s+/);
                let row = $("<tr>");
                $.each(parts, function(j, p) { row.append($("<td>").text(p)); });
                row.append($("<td>").text(c.file));
                row.append($("<td>").text(c.desc || ""));
                let tog = $("<button>").text("Toggle").addClass("small-btn")
                    .click(function() {
                        let enable = confirm("Enable this cron job?") ? 1 : 0;
                        $.post("/supermon/custom/toggle_cron.php", {raw_line: c.raw, enable: enable}, function(resp) {
                            alert(resp);
                            loadCronList();
                        });
                    });
                row.append($("<td>").append(tog));
                let acts = $("<td>");
                $("<button>").text("Play").addClass("small-btn play").click(function(){
                    $.post("/supermon/custom/run_announcement.php", {file: c.file}, function(r){ alert(r); });
                }).appendTo(acts);
                $("<button>").text("Edit").addClass("small-btn edit").click(function(){
                    let min = prompt("Minute", parts[0]); if(min===null)return;
                    let hour = prompt("Hour", parts[1]); if(hour===null)return;
                    let dom = prompt("Day", parts[2]); if(dom===null)return;
                    let month = prompt("Month", parts[3]); if(month===null)return;
                    let dow = prompt("DOW", parts[4]); if(dow===null)return;
                    $.post("/supermon/custom/update_announcement.php", {
                        raw_line: c.raw, min, hour, dom, month, dow
                    }, function(resp){ alert(resp); loadCronList(); });
                }).appendTo(acts);
                $("<button>").text("Delete").addClass("small-btn del").click(function(){
                    if(!confirm("Delete cron for "+c.file+"?")) return;
                    $.post("/supermon/custom/delete_announcement.php", {raw_line: c.raw}, function(r){
                        alert(r); loadCronList();
                    });
                }).appendTo(acts);
                row.append(acts);
                tbody.append(row);
            });
        }).fail(function() { console.error("Failed to load cron list"); });
    }
    loadCronList();

    // Install Announcement
    $("#install_announcement").click(function() {
        let mp3 = $("#mp3_select").val();
        if (!mp3) { alert("Please select an MP3 file."); return; }
        let min = prompt("Minute: Which minute of the hour? ", "45"); if(min===null) return;
        let hour = prompt("Hour: Which hours do you want? * = every hour or a specific like 8,13,20 or a range like 8-20", "8-21"); if(hour===null) return;
        let dom = prompt("DOM: Which days of the month? * is everyday or a specific like 1,7,18 or 1-17 as a range", "1-24"); if(dom===null) return;
        let month = prompt("Month: Which month(s) * = every or specific like 3 or a range like 7-10", "*"); if(month===null) return;
        let dow = prompt("DOW: Which day(s) of the week? Sun=1 Sat =7 you can choose all * or a specific or a range", "*"); if(dow===null) return;
        let desc = prompt("Description: Name this announcement", "Scheduled announcement");
        if (!desc || desc.trim()==="") {
            alert("Description required.");
            return;
        }
        $.post("/supermon/custom/announcement.php", {
            file: mp3,
            min: min,
            hour: hour,
            dom: dom,
            month: month,
            dow: dow,
            desc: desc
        }, function(data) {
            alert(data);
            loadULDropdown();
            loadCronList();
            loadMP3Dropdown();
        });
    });

    // Delete MP3/WAV
    $("#delete_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f || !confirm("Delete " + f + "?")) return;
        $.post("/supermon/custom/delete_file.php", {type:"mp3", file:f}, function(r){
            alert(r);
            loadMP3Dropdown();
        });
    });

    // NEW: Local Play for MP3/WAV (mirrors run_ul / play now behavior)
    $("#local_play_mp3").click(function() {
        let f = $("#mp3_select").val();
        if (!f) { 
            alert("Select an MP3/WAV file first"); 
            return; 
        }
        $.post("/supermon/custom/run_announcement.php", {file:f}, function(r){ 
            alert(r); 
        }).fail(function() {
            alert("Local play request failed - check server logs or permissions.");
        });
    });

    // Global Play for MP3/WAV with warning confirmation
    $("#global_play_mp3").click(function() {
        let btn = $(this);
        if (btn.prop('disabled')) return;
        btn.prop('disabled', true).text("Playing...");

        let f = $("#mp3_select").val();
        if (!f) {
            alert("Select an MP3/WAV file first");
            btn.prop('disabled', false).text("Global Play");
            return;
        }

        if (!confirm("WARNING: This announcement will play on this node AND all currently linked nodes.\n\nContinue?")) {
            btn.prop('disabled', false).text("Global Play");
            return;
        }

        $.post("/supermon/custom/globalplay.php", {file: f}, function(r){
            alert(r);
        }).fail(function() {
            alert("Global play request failed - check server logs or permissions.");
        }).always(function() {
            btn.prop('disabled', false).text("Global Play");
        });
    });

    // Delete .ul
    $("#delete_ul").click(function() {
        let f = $("#ul_select").val();
        if (!f || !confirm("Delete " + f + "?")) return;
        $.post("/supermon/custom/delete_file.php", {type:"ul", file:f}, function(r){
            alert(r);
            loadULDropdown();
        });
    });

    // Play Now (local node only) - for .ul files
    $("#run_ul").click(function() {
        let f = $("#ul_select").val();
        if (!f) { alert("Select announcement first"); return; }
        $.post("/supermon/custom/run_announcement.php", {file:f}, function(r){ alert(r); });
    });

    // Generate TTS
    $("#generate_tts").click(function() {
        let text = $("#tts_text").val().trim();
        let fn = $("#tts_filename").val().trim();
        if (!text || !fn) { alert("Text and filename required"); return; }
        if (!/^[a-zA-Z0-9_-]+$/.test(fn)) {
            alert("Filename: letters, numbers, hyphen or underscore only");
            return;
        }
        $.post("/supermon/custom/piper_generate.php", {text: text, filename: fn}, function(resp) {
            $("#tts_status").text(resp);
            if (resp.includes("Success")) {
                setTimeout(loadMP3Dropdown, 1500);
            }
        });
    });

});
</script>

<style>
    .small-btn { font-size:0.9em; padding:4px 8px; margin:0 3px; cursor:pointer; }
    .play { background:#17a2b8; color:white; border:none; }
    .edit { background:#ffc107; color:black; border:none; }
    .del { background:#dc3545; color:white; border:none; }
    .submit { padding:6px 12px; margin:4px; }
    #local_play_mp3:hover { opacity:0.85; }
    #global_play_mp3:hover { opacity:0.85; }
</style>
