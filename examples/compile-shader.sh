mkdir -p ${DST}/SPIRV
mkdir -p ${DST}/MSL
mkdir -p ${DST}/DXIL

for filename in $SRC/*.vert.hlsl; do
    if [ -f "$filename" ]; then
        echo -e "\e[94mCompile file: \e[94m${filename}\e[0m"
        BASE_NAME=$(basename ${filename})
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.spv}"
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.msl}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.dxil}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.json}"
    fi
done

for filename in $SRC/*.frag.hlsl; do
    if [ -f "$filename" ]; then
        echo -e "\e[94mCompile file: \e[94m${filename}\e[0m"
        BASE_NAME=$(basename ${filename})
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.spv}"
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.msl}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.dxil}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.json}"
    fi
done

for filename in $SRC/*.comp.hlsl; do
    if [ -f "$filename" ]; then
        echo -e "\e[94mCompile file: \e[94m${filename}\e[0m"
        BASE_NAME=$(basename ${filename})
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.spv}"
        $COMPILER "$filename" -o "${DST}/SPIRV/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.msl}"
        $COMPILER "$filename" -o "${DST}/MSL/${BASE_NAME/.hlsl/.json}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.dxil}"
        $COMPILER "$filename" -o "${DST}/DXIL/${BASE_NAME/.hlsl/.json}"
    fi
done
