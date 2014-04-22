function table2json(t)  
        local function serialize(tbl)  
                local tmp = {}  
                for k, v in pairs(tbl) do  
                        local k_type = type(k)  
                        local v_type = type(v)  
                        local key = (k_type == "string" and "\"" .. k .. "\":")  
                            or (k_type == "number" and "")  
                        local value = (v_type == "table" and serialize(v))  
                            or (v_type == "boolean" and tostring(v))  
                            or (v_type == "string" and "\"" .. v .. "\"")  
                            or (v_type == "number" and v)  
                        tmp[#tmp + 1] = key and value and tostring(key) .. tostring(value) or nil  
                end  
                if table.maxn(tbl) == 0 then  
                        return "{" .. table.concat(tmp, ",") .. "}"  
                else  
                        return "[" .. table.concat(tmp, ",") .. "]"  
                end  
        end  
        assert(type(t) == "table")  
        return serialize(t)  
end

function posStr(xx, yy)
	return string.format("{%f,%f}",xx,yy)
end


function rectStr(xx, yy,ww,hh)
	return string.format("{%f,%f,%f,%f}",xx,yy,ww,hh)
end
