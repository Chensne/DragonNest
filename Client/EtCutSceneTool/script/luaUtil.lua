
-- 테이블을 루아 파일로 익스포트
function fileOpen( file_name )
    FileHandle = io.open( file_name, "w+" )
end


function fileClose()
    io.close( FileHandle )
end


-- 테이블 깊이
tableDeep = -1

function printTab( offset )
	local count = tableDeep+offset
    for i = 1, count do
        FileHandle:write( string.format( "\t" ) ) 
    end
end 

function table_to_luafile( table_name, t )
    local t = t or _G[ table_name ] -- 루아의 모든 함수와 테이블은 _G 전역 테이블의 요소가 된다.
   
    tableDeep = tableDeep + 1     
    if tableDeep == 0 then
        FileHandle:write( string.format( "%s = {}\n\n", table_name ) )
    end
    
    -- 테이블 이름이 문자열일 경우엔 문자열로, 숫자일 경우엔 아무것도 적지 않는다.
    if type(table_name) == "string" then        
        printTab( 0 )
        FileHandle:write( string.format("%s = \n", table_name) )
    end
    
   
    printTab( 0 )
    FileHandle:write( string.format("{ \n") )
   
    for k, v in pairs(t) do
        if type(v) == "string" then
            v = string.format( "%q", v ) -- %q 는 루아 인터프리터가 읽을 수 있는 문자열로 컨버팅해준다.
        end
        if type(v) == "boolean" then
            if v then 
                v = "true"
            else 
                v = "false"
            end
        end
        
        if type(v) == "table" then -- 값이 테이블일 때
            table_to_luafile( k, v ) -- 키 값이 문자가 될 수도 있고 숫자가 될 수도 있고.
        elseif type(k) == "number" then -- 키가 숫자형일때,
            printTab( 1 )
            FileHandle:write( string.format( "%f,\n", v ) )
        elseif type(v) == "number"  then -- 값이 숫자형일때,
           printTab( 1 )
           FileHandle:write( string.format( "%s = %f,\n", k, v ) )
        else
           printTab( 1 )
           FileHandle:write( string.format( "%s = %s,\n", k, v ) )
        end
    end
    
    printTab( 0 )
    FileHandle:write( "}" )
    tableDeep = tableDeep - 1
    
    if tableDeep ~= -1 then
        FileHandle:write( "," )
    end
    
    FileHandle:write( "\n" )
end


-- 정확한 테이블 요소의 갯수 얻기
function GetTableSize( table )
    local size = 0
    for _, _ in pairs(table) do
        size = size + 1
    end
    
    return size
end


-- 콜 스택을 리턴한다.
function CallStack()
    local info = debug.getinfo( 3 )
    return info.source..":"..info.currentline.." "..info.name.."()"
end



--        if type(v) == "table" then
--            if type(k) == "number" then
--               table_to_luafile( table_name .. "[" .. k .. "]", v )
--            else
--                table_to_luafile( table_name .. "." .. k, v )
--            end
--        else
--           if type(k) == "number" then
--                print( table_name .. "[" ..k.. "] = " .. v )
--            else
--                print( table_name .. "." .. k .. " = " .. v )
--           end
--        end 
--    end

-- 다음과 같이 출력해 보자
-- tt = {
--          100,
--          {
--             1,
--             2, 
--             3
--          },
--          inner = {
--                     100, 
--                     "문자열",
--                     x = 10,
--                     alive = false
--                  },
--          x = 10
--      }