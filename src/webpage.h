const char MAIN_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html lang="en">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=   device-width, initial-scale=1.0">
    <title>Lifechanging Lights Control</title>
    <style>
        html {
            min-height: 100%;
            /* Unlike 'height', which is used by
                                        @Paulie_D, this will make your webpage
                                        automatically resize when it exceeds
                                        the 100% height, thus it won't start the
                                        gradient over
                                      */
        }

        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 20px;
            position: fixed;
            width: 400px;
            height: 800px;
            background: linear-gradient(90deg, rgba(71,71,71,1) 11%, rgba(196,195,195,1) 100%);
        }

        h2 {
            color: aliceblue;
        }

        .light-control {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .slider {
            width: 80%;
        }

        .button {
            display: block;
            width: 15%;
            padding: 10px;
            margin: 5px;
            font-size: 16px;
        }

        .control-wrap {
            display: flex;
            width: 100%;
        }

        .row-label {
            width: 100%;
            padding-top: 3%;
            padding-right: 40px;
            color: white;

        }

        input[type=checkbox] {
            height: 0;
            width: 0;
            visibility: hidden;
        }

        .toggle {
            cursor: pointer;
            text-indent: -9999px;
            width: 60px;
            height: 30px;
            background: grey;
            display: block;
            border-radius: 100px;
            position: relative;
        }

        .toggle:after {
            content: '';
            position: absolute;
            top: 1px;
            left: 1px;
            width: 28px;
            height: 28px;
            background: #fff;
            border-radius: 28px;
            transition: 0.3s;
        }

        input:checked+label {
            background: #13c400;
        }

        input:checked+label:after {
            left: calc(100% - 1px);
            transform: translateX(-100%);
        }

        label:active:after {
            width: 20px;
        }
    </style>
</head>

<body>
    <h2>Life-changing Lights Control</h2>
    <div class="light-control">
        <label class="row-label" for="slider1">Kitchen</label>
        <div class="control-wrap">
            <input type="range" min="0" max="1023" value="0" class="slider" id="slider1"
                oninput="updateBrightness(0, this.value)">
            <input type="checkbox" id="switch" /><label class="toggle" for="switch">Toggle</label>
        </div>
        <label class="row-label" for="slider2">Kitchen Light warmth</label>
        <div class="control-wrap">
            <input type="range" min="0" max="1023" value="0" class="slider" id="slider1"
                oninput="updateBrightness(0, this.value)">
            <input type="checkbox" id="switch1" /><label class="toggle" for="switch1">Toggle</label>
        </div>
        <label class="row-label" for="slider3">Livingroom</label>
        <div class="control-wrap">
            <input type="range" min="0" max="1023" value="0" class="slider" id="slider1"
                oninput="updateBrightness(0, this.value)">
            <input type="checkbox" id="switch2" /><label class="toggle" for="switch2">Toggle</label>
        </div>
        <label class="row-label" for="slider4">Livingroom Light warmth</label>
        <div class="control-wrap">
            <input type="range" min="0" max="1023" value="0" class="slider" id="slider1"
                oninput="updateBrightness(0, this.value)">
            <input type="checkbox" id="switch3" /><label class="toggle" for="switch3">Toggle</label>
        </div>
    </div>

    <script>
        function updateBrightness(light, value) {
            document.getElementById(`slider${light + 1}`).value = value;
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/set?light=' + light + '&brightness=' + value, true);
            xhr.send();
        }

        function toggleLight(light) {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/toggle?light=' + light, true);
            xhr.send();
        }

        function updateLightState(value) {
            document.getElementById("lightState[0]").innerHTML = "Sensor Value: " + value;
        }
    </script>
</body>

</html>
)=====";
