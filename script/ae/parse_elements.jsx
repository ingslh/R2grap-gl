{
  function ReplaceAll(str, substr, newstr) {
    var p = -1; 
    var s = 0; 

    while ((p = str.indexOf(substr, s)) > -1) {
        s = p + newstr.length;
        str = str.replace(substr, newstr);
    }
    return str;
  }

  function normalstr(str) {
      str = ReplaceAll(str, ">", "&gt;");
      str = ReplaceAll(str, "<", "&lt;");
      str = ReplaceAll(str, "\"", "&quot;");
      str = ReplaceAll(str, "\'", "&#39;");
      str = ReplaceAll(str, "\n", "\\n");
      str = ReplaceAll(str, "\r", "\\r");
      return str
  }

  function getCurveLength(initPos,endPos, outBezier, inBezier){
    var k, curveSegments = 200;
    var point,lastPoint = null;
    var ptDistance;
    var absToCoord,absTiCoord;
    var triCoord1,triCoord2,triCoord3,liCoord1,liCoord2,ptCoord,perc,addedLength = 0;
    for(k=0;k<curveSegments;k+=1){
        point = [];
        perc = k/(curveSegments-1);
        ptDistance = 0;
        absToCoord = [];
        absTiCoord = [];
        outBezier.forEach(function(item,index){
            if(absToCoord[index] == null){
                absToCoord[index] = initPos[index] + outBezier[index];
                absTiCoord[index] = endPos[index] + inBezier[index];
            }
            triCoord1 = initPos[index] + (absToCoord[index] - initPos[index])*perc;
            triCoord2 = absToCoord[index] + (absTiCoord[index] - absToCoord[index])*perc;
            triCoord3 = absTiCoord[index] + (endPos[index] - absTiCoord[index])*perc;
            liCoord1 = triCoord1 + (triCoord2 - triCoord1)*perc;
            liCoord2 = triCoord2 + (triCoord3 - triCoord2)*perc;
            ptCoord = liCoord1 + (liCoord2 - liCoord1)*perc;
            point.push(ptCoord);
            if(lastPoint !== null){
                ptDistance += Math.pow(point[index] - lastPoint[index],2);
            }
        });
        ptDistance = Math.sqrt(ptDistance);
        addedLength += ptDistance;
        lastPoint = point;
    }
    return addedLength;
  }

  if (!Array.prototype.forEach) {
    Array.prototype.forEach = function(callback, thisArg) {
        var T, k;
        if (this == null) {
            throw new TypeError(' this is null or not defined');
        }
        var O = Object(this);
        var len = O.length >>> 0;
        if (typeof callback !== "function") {
            throw new TypeError(callback + ' is not a function');
        }
        if (arguments.length > 1) {
            T = thisArg;
        }
        k = 0;
        while (k < len) {
            var kValue;
            if (k in O) {
                kValue = O[k];
                callback.call(T, kValue, k, O);
            }
            k++;
        }
    };
}

  if (!Window.JSON) {
    JSON = {
        parse: function (jsonStr) {
            return eval('(' + jsonStr + ')');
        },
        stringify: function (jsonObj) {
            var result = '',
                curVal;
            if (jsonObj === null) {
                return String(jsonObj);
            }
            switch (typeof jsonObj) {
                case 'number':
                case 'boolean':
                    return String(jsonObj);
                case 'string':
                    return '"' + normalstr(jsonObj.toString()) + '"';
                case 'undefined':
                case 'function':
                    return undefined;
            }

            switch (Object.prototype.toString.call(jsonObj)) {
                case '[object Array]':
                    result += '[';
                    for (var i = 0, len = jsonObj.length; i < len; i++) {
                        curVal = JSON.stringify(jsonObj[i]);
                        result += (curVal === undefined ? null : curVal) + ",";
                    }
                    if (result !== '[') {
                        result = result.slice(0, -1);
                    }
                    result += ']';
                    return result;
                case '[object Date]':
                    return '"' + (jsonObj.toJSON ? jsonObj.toJSON() : jsonObj.toString()) + '"';
                case '[object RegExp]':
                    return "{}";
                case '[object Object]':
                    result += '{';
                    for (i in jsonObj) {
                        if (jsonObj.hasOwnProperty(i)) {
                            curVal = JSON.stringify(jsonObj[i]);
                            if (curVal !== undefined) {
                                result += '"' + i + '":' + curVal + ',';
                            }
                        }
                    }
                    if (result !== '{') {
                        result = result.slice(0, -1);
                    }
                    result += '}';
                    return result;

                case '[object String]':
                    return '"' + normalstr(jsonObj.toString()) + '"';
                case '[object Number]':
                case '[object Boolean]':
                    return jsonObj.toString();
            }
        }
    };
  }

  if (!Object.keys) {
    Object.keys = (function () {
        var hasOwnProperty = Object.prototype.hasOwnProperty,
            hasDontEnumBug = !({ toString: null }).propertyIsEnumerable('toString'),
            dontEnums = [
                'toString',
                'toLocaleString',
                'valueOf',
                'hasOwnProperty',
                'isPrototypeOf',
                'propertyIsEnumerable',
                'constructor'
            ],
            dontEnumsLength = dontEnums.length;

        return function (obj) {
            if (typeof obj !== 'object' && typeof obj !== 'function' || obj === null) throw new TypeError('Object.keys called on non-object');

            var result = [];

            for (var prop in obj) {
                if (hasOwnProperty.call(obj, prop)) result.push(prop);
            }

            if (hasDontEnumBug) {
                for (var i = 0; i < dontEnumsLength; i++) {
                    if (hasOwnProperty.call(obj, dontEnums[i])) result.push(dontEnums[i]);
                }
            }
            return result;
        }
    })()
  };
  // -- Prototypes
  if (!String.matchWith) {
    String.prototype.matchWith = function (str) {
        return this.indexOf(str) >= 0;
    };
  }

  function createFile(path, flag) {
    var file = File(path);
    if (flag == undefined) {
        file.remove();
    }
    if (!file.exists) {
        file = new File(path);
    }
    return file;
  }

  function readFile(file) {
    file.open('r', undefined, undefined);
    var obj;
    if (file !== '') {
        var read = file.read();
        var content = read ? read : '{}';
        obj = JSON.parse(content);
        file.close();
    }
    return obj;
  }

  function writeTofile(file, str, wtype) {
    var out; // our output
    // we know already that the file exist
    // but to be sure
    if (file !== '') {
        //Open the file for writing.
        out = file.open(wtype, undefined, undefined);
        file.encoding = "UTF-8";
        // file.lineFeed = "Unix"; //convert to UNIX lineFeed
        file.lineFeed = "Windows";
        // txtFile.lineFeed = "Macintosh";
    }
    // got an output?
    if (out !== false) {
        // loop the list and write each item to the file
        file.write(str);
        // always close files!
        file.close();
    }
}

  function getSettingsPath() {
    var path = 'C:/Users/Public/.WEC.CACHE/Settings.json';
    var file = File(path);

    if (!file.exists) {
        file = new File(path);
    }
    return file;
  }

  function createFolder(path) {
    var folder = new Folder(path);
    if (!folder.exists) {
        folder.create();
    }
    return folder;
  }

  function parse_elements(thisObj) {
    // read setting file
    var mySetting = readFile(getSettingsPath());
    var separator = "//", outputUrl, rootFolder;
    var proFileName = "data.json" , pathFileName;

    function onSetupEffects() {
      // comp duplicate
      var previousComps = [];
      var jsonObj = {}, curObj = {};
      var write_file;
      var debugLogTxt = {};
      outputUrl = mySetting.export_path + separator;

      var comps = app.project.selection;
      if (comps.length == 0) {
        alert("Maybe the comps you selected has existed in renderQueue!");
      } else {
        classificationProcessing(comps);
      }

      function classificationProcessing(comps){
        // create output folder
        var folderName = 'AEC_outputs';
        if (app.project.file != null) {
            folderName = app.project.file.name.split(".")[0];
        }
        folderName = folderName.split("%20").join("_");
        pathFileName = folderName + ".json";
        outputUrl += folderName;
        rootFolder = createFolder(outputUrl);
        // create [project_name].json for writing path
        var filepath = outputUrl + separator + pathFileName;
        write_file = createFile(filepath, 0);
        // read [project_name].json
        debugLogTxt = readFile(write_file);

        for (var i = 0; i < comps.length; i++) {
          // Duplicate the supplied comp structure
          var comp = comps[i];
          jsonObj = {};
          debugLogTxt[comp.name] = {};
          //comp = tcd_duplicateCompStructure(comp, 1);

          // create comp folder
          var folderPath = outputUrl + separator + comp.name;
          createFolder(folderPath);
          // get comp and layer property and write it to file
          compPropertyToJson(comp);
          layerPropertyToJson(comp);
          var filepath = outputUrl + separator + comp.name + separator + proFileName;
          var file = createFile(filepath);
          // write to data.json,'w' means overwrite
          var propertyJson = JSON.stringify(jsonObj).split("\\").join("//");
          // convert [####] to \r\n
          propertyJson = propertyJson.replace(/(\[####\])/g, '\\r\\n');
          writeTofile(file, propertyJson, 'w');
          debugLogTxt[comp.name]["json"] = filepath;
        }
      }
      function compPropertyToJson(comp) {
        jsonObj[comp.name] = {
          "width": comp.width,
          "height": comp.height,
          "pixelAspect": comp.pixelAspect,
          "frameRate": comp.frameRate,
          "frameDuration": comp.frameDuration,
          "duration": comp.duration,
          "time": comp.time,
          "dropFrame": comp.dropFrame,
          "numLayers": comp.numLayers,
          "motionBlur": comp.motionBlur,
          "draft3d": comp.draft3d,
          "frameBlending": comp.frameBlending,
          "preserveNestedFrameRate": comp.preserveNestedFrameRate,
          "activeCamera": comp.activeCamera,
          "displayStartTime": comp.displayStartTime,
          "shutterAngle": comp.shutterAngle,
          "shutterPhase": comp.shutterPhase,
          "renderer": comp.renderer,
          "Markers": []
        };
      }

      function layerPropertyToJson(comp) {
        jsonObj[comp.name]["layers"] = [];
        for (var i = 1; i <= comp.numLayers; i++) {
          var layer = comp.layer(i);
          curObj = {};
          // get Shapelayer property 
          if(layer instanceof ShapeLayer){
            curObj = {
              "index": layer.index,
              "name": layer.name,
              "blendingMode": layer.blendingMode,
              "Parent&Link": layer.parent ? layer.parent.index : "",
              "startTime": layer.startTime,
              "outPoint": layer.outPoint,
              "inPoint": layer.inPoint
            };
            for (var ii = 1; ii <= layer.numProperties; ii++) {
              var name = layer.property(ii).name;
              if(name == "Layer Styles" || name == "Geometry Options" || 
                 name == "Material Options" || name == "Audio" || name == "Marker") 
                continue;
              curObj[name] = {};
              iterateLayerProperty("shapeLayer", layer[name], curObj[name]);
            }
          }
          if (Object.keys(curObj).length > 0) {
            jsonObj[comp.name]["layers"].push(curObj);
          }
        }
      }
    }



    function buildSegment(property, indexTime){
      var averageSpeed, duration;
      var bezierIn, bezierOut;
      function buildKeyInfluence(key,lastKey, indexTime){
        switch(property.propertyValueType){
          case PropertyValueType.ThreeD_SPATIAL:
          case PropertyValueType.TwoD_SPATIAL:
          case PropertyValueType.SHAPE:
              key.easeIn = {
                  influence : property.keyInTemporalEase(indexTime+1)[0].influence,
                  speed : property.keyInTemporalEase(indexTime+1)[0].speed
              };
              lastKey.easeOut = {
                  influence : property.keyOutTemporalEase(indexTime)[0].influence,
                  speed : property.keyOutTemporalEase(indexTime)[0].speed
              };
              break;
          default:
              key.easeIn = [];
              lastKey.easeOut = [];
              var inEase = property.keyInTemporalEase(indexTime+1);
              var outEase = property.keyOutTemporalEase(indexTime);
              inEase.forEach(function(item,index){
                  key.easeIn.push({influence : item.influence, speed:item.speed});
                  lastKey.easeOut.push({influence : outEase[index].influence, speed:outEase[index].speed});
              });
        }
      }
      var i, len;
      var key = {};
      var lastKey = {};
      var interpolationType = '';
      key.time = property.keyTime(indexTime+1);
      lastKey.time = property.keyTime(indexTime);
      key.value = property.keyValue(indexTime+1);
      lastKey.value = property.keyValue(indexTime);
      if(!(key.value instanceof Array)){
        key.value = [key.value];
        lastKey.value = [lastKey.value];
      } 
      if(property.keyOutInterpolationType(indexTime) == KeyframeInterpolationType.HOLD){
        interpolationType = 'hold';
      }else{
        if(property.keyOutInterpolationType(indexTime) == KeyframeInterpolationType.LINEAR && property.keyInInterpolationType(indexTime + 1) == KeyframeInterpolationType.LINEAR){
          interpolationType = 'linear';
        }
        buildKeyInfluence(key, lastKey, indexTime);
        switch(property.propertyValueType){
          case PropertyValueType.ThreeD_SPATIAL:
          case PropertyValueType.TwoD_SPATIAL:
              lastKey.to = property.keyOutSpatialTangent(indexTime);
              key.ti = property.keyInSpatialTangent(indexTime+1);
              break;
        }
      }
      if(interpolationType == 'hold'){
        berizerIn = {x:0, y:0};
        berizerOut = {x:0, y:0};
        return;
      }
      duration = key.time - lastKey.time;
      len = key.value.length;
      bezierIn = {};
      bezierOut = {};
      averageSpeed = 0;
      var infOut,infIn;
      switch(property.propertyValueType){
        case PropertyValueType.ThreeD_SPATIAL:
        case PropertyValueType.TwoD_SPATIAL:
          var curveLength = getCurveLength(lastKey.value,key.value, lastKey.to, key.ti);
          averageSpeed = curveLength/duration;
          if(curveLength === 0){
            infOut = lastKey.easeOut.influence;
            infIn = key.easeIn.influence;
          }else{
            infOut = Math.min(100*curveLength/(lastKey.easeOut.speed*duration), lastKey.easeOut.influence);
            infIn = Math.min(100*curveLength/(key.easeIn.speed*duration), key.easeIn.influence);
          }
          bezierIn.x = 1 - infIn / 100;
          bezierOut.x = infOut / 100;
          break;
        case PropertyValueType.ThreeD:
        case PropertyValueType.TwoD:
        case PropertyValueType.OneD:
        case PropertyValueType.COLOR:
          bezierIn.x = [];
          bezierOut.x = [];
          key.easeIn.forEach(function(item, index){
              bezierIn.x[index] = 1 - item.influence / 100;
              bezierOut.x[index] = lastKey.easeOut[index].influence / 100;

          });
          averageSpeed = [];
          for(i=0;i<len;i+=1){
              averageSpeed[i] =  (key.value[i] - lastKey.value[i])/duration;
          }
          break;
      }
      if(averageSpeed == 0){
        berizerIn.y = berizerIn,x;
        berizerOut.y = berizerOut.x;
      }else{
        switch(property.propertyValueType){
          case PropertyValueType.ThreeD_SPATIAL:
          case PropertyValueType.TwoD_SPATIAL:
          case PropertyValueType.SHAPE:
            if(interpolationType == 'linear'){
              bezierIn.y = bezierIn.x;
              bezierOut.y = bezierOut.x;
            }else{
              bezierIn.y =  1 - ((key.easeIn.speed) / averageSpeed) * (infIn / 100);
              bezierOut.y = ((lastKey.easeOut.speed) / averageSpeed) * bezierOut.x;
            }
            break;
          case PropertyValueType.ThreeD:
          case PropertyValueType.TwoD:
          case PropertyValueType.OneD:
          case PropertyValueType.COLOR:
            bezierIn.y = [];
            bezierOut.y = [];
            key.easeIn.forEach(function(item,index){
                if(averageSpeed[index] == 0 || interpolationType == 'linear'){
                    bezierIn.y[index] = bezierIn.x[index];
                    bezierOut.y[index] = bezierOut.x[index];
                }else{
                    bezierIn.y[index] = 1 - ((item.speed) / averageSpeed[index]) * (item.influence / 100);
                    bezierOut.y[index] = ((lastKey.easeOut[index].speed) / averageSpeed[index]) * bezierOut.x[index];
                }
            });
            break;
        }
      }
      return [bezierOut, bezierIn];
    }

    function iterateLayerProperty(layerType, pro, jObj) {
      if (pro.numProperties && pro.numProperties > 0){
        for (var i = 1; i <= pro.numProperties; i++){
          var cur = pro(i);
          if (cur.numProperties && cur.numProperties > 0) {
            jObj[cur.name] = {};
            iterateLayerProperty(layerType, cur, jObj[cur.name]);
          }
          else{
            // some value is readOnly or other type,via check propertyValueType
            if ((cur.propertyValueType != PropertyValueType.NO_VALUE) && cur.propertyValueType != PropertyValueType.CUSTOM_VALUE){
              if (cur.numKeys > 0) {

                jObj[cur.name] = {};
                jObj[cur.name]["value"] = cur.value;
                for (var j = 1; j < cur.numKeys; j++) {
                  out_in_dir = buildSegment(cur, j);
                  jObj[cur.name]["Curve" + j] = {
                    "lastkeyValue": cur.keyValue(j),
                    "lastkeyTime": cur.keyTime(j),
                    "OutPos": out_in_dir[0],
                    "InPos": out_in_dir[1],
                    "keyValue":cur.keyValue(j+1),
                    "keyTime":cur.keyTime(j+1),
                  };
                }
              }else{
                jObj[cur.name] = cur.value;
              }
            }
          }
          if(layerType == "shapeLayer" && cur.matchName == 'ADBE Vector Shape - Group'){
            jObj[cur.name]["OutPos"] = cur.property('Path').value.outTangents;
            jObj[cur.name]["InPos"] = cur.property('Path').value.inTangents;
            jObj[cur.name]["Vertices"] = cur.property('Path').value.vertices;
            jObj[cur.name]["Closed"] = cur.property('Path').value.closed;
          }
        }
      }else if(pro.propertyValueType == PropertyValueType.MARKER) {
        var obj = pro.value;
        var keys = Object.keys(obj);
        for (var i = 0; i < keys.length; i++) {
          jObj[keys[i]] = obj[keys[i]];
        }
      }
    }    
    onSetupEffects();
  }
  parse_elements(this);
  alert('export successful!');
}
