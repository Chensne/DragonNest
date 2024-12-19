
-- ���̺��� ��� ���Ϸ� �ͽ���Ʈ
function fileOpen( file_name )
    FileHandle = io.open( file_name, "w+" )
end


function fileClose()
    io.close( FileHandle )
end


-- ���̺� ����
tableDeep = -1

function printTab( offset )
	local count = tableDeep+offset
    for i = 1, count do
        FileHandle:write( string.format( "\t" ) ) 
    end
end 

function table_to_luafile( table_name, t )
    local t = t or _G[ table_name ] -- ����� ��� �Լ��� ���̺��� _G ���� ���̺��� ��Ұ� �ȴ�.
   
    tableDeep = tableDeep + 1     
    if tableDeep == 0 then
        FileHandle:write( string.format( "%s = {}\n\n", table_name ) )
    end
    
    -- ���̺� �̸��� ���ڿ��� ��쿣 ���ڿ���, ������ ��쿣 �ƹ��͵� ���� �ʴ´�.
    if type(table_name) == "string" then        
        printTab( 0 )
        FileHandle:write( string.format("%s = \n", table_name) )
    end
    
   
    printTab( 0 )
    FileHandle:write( string.format("{ \n") )
   
    for k, v in pairs(t) do
        if type(v) == "string" then
            v = string.format( "%q", v ) -- %q �� ��� ���������Ͱ� ���� �� �ִ� ���ڿ��� ���������ش�.
        end
        if type(v) == "boolean" then
            if v then 
                v = "true"
            else 
                v = "false"
            end
        end
        
        if type(v) == "table" then -- ���� ���̺��� ��
            table_to_luafile( k, v ) -- Ű ���� ���ڰ� �� ���� �ְ� ���ڰ� �� ���� �ְ�.
        elseif type(k) == "number" then -- Ű�� �������϶�,
            printTab( 1 )
            FileHandle:write( string.format( "%f,\n", v ) )
        elseif type(v) == "number"  then -- ���� �������϶�,
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


-- ��Ȯ�� ���̺� ����� ���� ���
function GetTableSize( table )
    local size = 0
    for _, _ in pairs(table) do
        size = size + 1
    end
    
    return size
end


-- �� ������ �����Ѵ�.
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

-- ������ ���� ����� ����
-- tt = {
--          100,
--          {
--             1,
--             2, 
--             3
--          },
--          inner = {
--                     100, 
--                     "���ڿ�",
--                     x = 10,
--                     alive = false
--                  },
--          x = 10
--      }