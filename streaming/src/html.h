#ifndef HTML_H
#define HTML_H

const char* htmlIndex = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Sonatino Streaming Example</title>
    <style>
      :root {
        --main-bg-color: #1a232e;
        --container-bg-color: #26303c;
        --text-color: #eaeaea;
        --accent-color: #ea6d33;
        --input-bg-color: #1d2733;
        --hover-bg-color: #385981;
        --disabled-bg-color: #4e4e50;
        --disabled-text-color: #7c7c7c;
        --range-track-color: #505050;
      }
      body {
        background-color: var(--main-bg-color);
        color: var(--text-color);
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
        margin: 0;
        font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
      }
      h2,
      a,
      button,
      input[type="range"] {
        margin: 0;
      }
      h2 {
        margin-bottom: 10px;
      }
      a {
        color: var(--accent-color);
        text-decoration: none;
      }
      .bold {
        font-weight: bold;
      }
      .container {
        text-align: center;
        padding: 20px;
        background-color: var(--container-bg-color);
        border-radius: 10px;
        box-shadow: 0 0 15px rgba(0, 0, 0, 0.5);
        margin: 20px;
        width: calc(100% - 40px);
        max-width: 500px;
      }
      input,
      button {
        border-radius: 5px;
        font-size: 16px;
        outline: none;
      }
      input[type="text"],
      select {
        padding: 10px;
        margin: 10px;
        background-color: var(--input-bg-color);
        border: 1px solid transparent;
        color: var(--text-color);
        width: calc(100% - 40px);
      }
      select {
        font-size: 100%;
      }
      input[type="range"] {
        -webkit-appearance: none;
        appearance: none;
        width: 100%;
        cursor: pointer;
        outline: none;
        height: 8px;
        background: var(--range-track-color);
        transition: background-color 0.3s ease;
      }
      input[type="range"]:hover {
        background: var(--hover-bg-color);
      }
      input[type="range"]::-webkit-slider-thumb {
        -webkit-appearance: none;
        height: 18px;
        width: 18px;
        background-color: var(--accent-color);
        border-radius: 50%;
        transition: transform 0.3s ease;
      }
      input[type="range"]::-webkit-slider-thumb:hover {
        transform: scale(1.15);
      }
      input[type="range"]::-moz-range-thumb {
        height: 18px;
        width: 18px;
        background-color: var(--accent-color);
        border-radius: 50%;
        transition: transform 0.3s ease;
      }
      input[type="range"]::-moz-range-thumb:hover {
        transform: scale(1.15);
      }
      button {
        padding: 10px 20px;
        background-color: var(--accent-color);
        border: 1px solid transparent;
        color: var(--text-color);
        cursor: pointer;
        transition: background-color 0.3s ease;
      }
      button:hover {
        border: 1px solid var(--text-color);
      }
      button:active {
        background-color: var(--hover-bg-color);
      }
      button:disabled {
        background-color: var(--disabled-bg-color);
        color: var(--disabled-text-color);
        cursor: not-allowed;
      }
      label {
        margin-top: 20px;
        font-size: 0.9em;
      }
      .topMargin {
        margin-top: 20px;
      }
      .description {
        color: #888;
        font-size: 14px;
        margin-bottom: 30px;
      }
      #infoContainer {
        margin-bottom: 20px;
      }
      #findMore {
        font-size: 70%;
      }
      #urlInput {
        border: 1px solid var(--text-color);
      }
      #log {
        margin-top: 10px;
        padding: 20px;
        background-color: var(--input-bg-color);
        color: var(--disabled-text-color);
        border-radius: 5px;
        height: 200px;
        overflow-y: auto;
        font-family: monospace;
        white-space: pre;
        text-align: left;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h2>Sonatino Streaming Example</h2>
      <p class="description">Supports MP3 and AAC streams over HTTP/HTTPS</p>
      <div id="infoContainer">
        <div id="titleContainer">
          Title: <span class="bold" id="title"></span>
        </div>
        <div id="stationContainer">
          Station: <span class="bold" id="station"></span>
        </div>
        <div id="artistContainer">
          Artist: <span class="bold" id="artist"></span>
        </div>
      </div>
      <div id="randomStream">
        <select name="genre" id="genre" onchange="getStation(this.value)">
          <option value="">Select a random station by genre</option>
          <option value="ambient">Ambient</option>
          <option value="blues">Blues</option>
          <option value="classical">Classical</option>
          <option value="country">Country</option>
          <option value="dance">Dance</option>
          <option value="dubstep">Dubstep</option>
          <option value="electronic">Electronic</option>
          <option value="folk">Folk</option>
          <option value="funk">Funk</option>
          <option value="hip hop">Hip Hop</option>
          <option value="house">House</option>
          <option value="jazz">Jazz</option>
          <option value="latin">Latin</option>
          <option value="metal">Metal</option>
          <option value="pop">Pop</option>
          <option value="r&b">R&amp;B</option>
          <option value="rap">Rap</option>
          <option value="reggae">Reggae</option>
          <option value="rock">Rock</option>
          <option value="soul">Soul</option>
          <option value="techno">Techno</option>
          <option value="trance">Trance</option>
        </select>
      </div>
      <div id="searching">Searching...</div>
      <div id="findMore">
        <a
          href="http://dir.xiph.org/codecs/MP3"
          target="_blank"
          rel="noopener noreferrer"
          >Find More Stations</a
        >
      </div>
      <div class="topMargin">
        <input id="urlInput" type="text" placeholder="Stream URL" />
      </div>
      <button id="startStream">Start</button>
      <button id="pauseResume">Pause</button>
      <button id="stopStream">Stop</button>
      <div class="topMargin">
        <label for="volumeControl">Volume: <span id="volValue"></span></label>
        <input type="range" id="volumeControl" min="0" max="21" />
      </div>
      <div class="topMargin">Event Log</div>
      <div id="log">(nothing yet)</div>
    </div>
    <script>
      const DEFAULT_VOLUME = 5;
      const STATION_LANGUAGE = "english";
      const RADIO_BROWSER_BASE = "http://all.api.radio-browser.info";
      const RADIO_BROWSER_HEADERS = {
        "User-Agent": "Sonatino +https://sonatino.com Streaming Example/1.0",
      };
      let isChangingVolume = false,
        abortControllers = {},
        lastGetStationId = "",
        lastGetStationUrl = "",
        log = "";
      const elements = {
        titleContainer: document.getElementById("titleContainer"),
        stationContainer: document.getElementById("stationContainer"),
        artistContainer: document.getElementById("artistContainer"),
        title: document.getElementById("title"),
        station: document.getElementById("station"),
        artist: document.getElementById("artist"),
        log: document.getElementById("log"),
        pauseResume: document.getElementById("pauseResume"),
        startStream: document.getElementById("startStream"),
        stopStream: document.getElementById("stopStream"),
        infoContainer: document.getElementById("infoContainer"),
        volumeControl: document.getElementById("volumeControl"),
        urlInput: document.getElementById("urlInput"),
        genreSelect: document.getElementById("genre"),
        randomStream: document.getElementById("randomStream"),
        searching: document.getElementById("searching"),
        volValue: document.getElementById("volValue"),
      };

      const updateVisibility = (elem, condition, isInline = false) => {
        const displayType = isInline ? "inline" : "block";
        elem.style.display = condition ? displayType : "none";
      };

      const updatePlayState = (isPlaying, isPaused) => {
        updateVisibility(elements.startStream, !isPlaying && !isPaused, true);
        updateVisibility(elements.pauseResume, isPlaying || isPaused, true);
        updateVisibility(elements.infoContainer, isPlaying);
        updateVisibility(elements.randomStream, !isPlaying && !isPaused);
        elements.pauseResume.innerText = isPaused ? "Resume" : "Pause";
        elements.startStream.disabled = isPlaying || isPaused;
        elements.pauseResume.disabled = !isPlaying && !isPaused;
      };

      const updateInfo = (data = { title: "", station: "", artist: "" }) => {
        const clearAll = !data.title && !data.station && !data.artist;
        if (data.title || clearAll) {
          updateVisibility(elements.titleContainer, data.title);
          elements.title.innerText = data.title;
        }
        if (data.station || clearAll) {
          updateVisibility(elements.stationContainer, data.station);
          elements.station.innerText = data.station;
        }
        if (data.artist || clearAll) {
          updateVisibility(elements.artistContainer, data.artist);
          elements.artist.innerText = data.artist;
        }
      };

      const updateVolume = (volume = 0) => {
        elements.volumeControl.value = volume;
        elements.volValue.innerText = `${Math.round((volume / 21) * 100)}%`;
      };

      const logMessage = (message) => {
        log += `${message}\r\n`;
        elements.log.innerText = log;
        elements.log.scrollTop = elements.log.scrollHeight;
      };

      const resetGenreSelect = () => {
        elements.genreSelect.value = "";
      };

      const eventSource = new EventSource("/events");
      eventSource.addEventListener("message", (event) => {
        const data = JSON.parse(event.data);
        switch (data.event) {
          case "status":
            const status = JSON.parse(data.data);
            updatePlayState(status.isPlaying, status.isPaused);
            updateInfo(status);
            if (!isChangingVolume) updateVolume(status.volume);
            break;
          case "log":
            logMessage(data.data);
            break;
        }
      });

      const sendDataToServer = async (data, requestType) => {
        try {
          if (abortControllers[requestType]) {
            abortControllers[requestType].abort();
          }
          abortControllers[requestType] = new AbortController();
          const timeoutSignal = AbortSignal.timeout(5000);
          const response = await fetch("/action", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(data),
            signal: AbortSignal.any([
              abortControllers[requestType].signal,
              timeoutSignal,
            ]),
          });
          if (response.status !== 200) {
            throw new Error(`HTTP response status: ${response.status}`);
          }
          return await response.json();
        } catch (error) {
          logMessage(`Error: ${error.message}`);
        }
      };

      const throttledSendDataToServer = throttle(sendDataToServer, 500);

      const getStation = async (genre) => {
        if (!genre) return;
        elements.urlInput.value = "";
        updateVisibility(elements.randomStream, false);
        updateVisibility(elements.searching, true);
        const url = `${RADIO_BROWSER_BASE}/json/stations/search?codec=mp3&limit=10&hidebroken=true&language=${STATION_LANGUAGE}&bitrateMax=128&order=clickcount&reverse=true&tag=${genre}`;
        try {
          const response = await fetch(url, { headers: RADIO_BROWSER_HEADERS });
          if (response.status !== 200) {
            throw new Error(`HTTP response status: ${response.status}`);
          }
          const data = await response.json();
          if (!data.length) {
            throw new Error("No stations found");
          }
          const station = data[Math.floor(Math.random() * data.length)];
          lastGetStationId = station.stationuuid;
          lastGetStationUrl = station.url;
          elements.urlInput.value = station.url;
        } catch (error) {
          logMessage(`Error fetching station info: ${error.message}`);
        }
        updateVisibility(elements.searching, false);
        updateVisibility(elements.randomStream, true);
      };

      elements.startStream.addEventListener("click", async () => {
        updateInfo();
        resetGenreSelect();
        elements.startStream.disabled = true;
        const url = elements.urlInput.value;
        const response = await sendDataToServer(
          { action: "open", url },
          "open"
        );
        if (response && response.success) {
          // Report station click
          if (
            lastGetStationId &&
            lastGetStationUrl &&
            lastGetStationUrl === url
          ) {
            await fetch(`${RADIO_BROWSER_BASE}/json/url/${lastGetStationId}`, {
              headers: RADIO_BROWSER_HEADERS,
            });
          }
        } else {
          elements.startStream.disabled = false;
        }
      });

      elements.stopStream.addEventListener("click", () => {
        resetGenreSelect();
        sendDataToServer({ action: "stop" }, "stop");
        updatePlayState(false, false);
      });

      elements.pauseResume.addEventListener("click", async () => {
        elements.pauseResume.disabled = true;
        const response = await sendDataToServer({ action: "pause" }, "pause");
      });

      elements.volumeControl.addEventListener("input", (event) => {
        const volume = event.target.value;
        updateVolume(volume);
        throttledSendDataToServer({ action: "volume", volume }, "volume");
      });

      elements.volumeControl.addEventListener("mousedown", () => {
        isChangingVolume = true;
      });

      elements.volumeControl.addEventListener("mouseup", () => {
        isChangingVolume = false;
      });

      updateVolume(DEFAULT_VOLUME);
      updatePlayState(false, false);
      updateInfo();
      updateVisibility(elements.searching, false);

      // Throttle function from https://github.com/pealm/pass.js
      // License: MIT (https://github.com/pealm/pass.js/blob/main/LICENSE)
      // (c) Copyright 2020 Pass JS, Bozdev, Onur Boz, (https://onurboz.com)
      function throttle(callback, wait, options = {}) {
        let context, args, result;
        let timeout = null;
        let previous = 0;
        const later = function () {
          previous = options.leading === false ? 0 : Date.now();
          timeout = null;
          result = callback.apply(context, args);
          if (!timeout) context = args = null;
        };
        return function () {
          const now = Date.now();
          if (!previous && options.leading === false) previous = now;
          const remaining = wait - (now - previous);
          context = this;
          args = arguments;
          if (remaining <= 0 || remaining > wait) {
            if (timeout) {
              clearTimeout(timeout);
              timeout = null;
            }
            previous = now;
            result = callback.apply(context, args);
            if (!timeout) context = args = null;
          } else if (!timeout && options.trailing !== false) {
            timeout = setTimeout(later, remaining);
          }
          return result;
        };
      }
    </script>
  </body>
</html>
)rawliteral";

#endif  // HTML_H