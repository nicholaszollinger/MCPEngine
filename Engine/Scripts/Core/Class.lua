-- Class.lua

-----------------------------------------------------------------------------------------------
--- Looks through the list of parents for an implementation of the function by the key value.
---@param key any
---@param parentList any
---@return any ClassImplementation The implementation for the function that matches the key. 
-----------------------------------------------------------------------------------------------
local function SearchForImplementation(key, parentList)
    for class, _ in pairs(parentList) do
        local implementation = parentList[class][key] -- try the 'i'th parent class.
        if implementation then
            return implementation; -- if we found it, return immediately.
        end
    end
end

function CreateClass(...)
    local class = {}; -- Table representing the behavior of the Widget.

    local classCount = select('#', ...);
    --print("Creating class from " .. classCount .. "classes");

    -- if we are inheriting from another class,
    if (classCount ~= 0) then
        -- If we are only inheriting functions from a single class, then just set the metatable
        -- to the single class
        if (classCount == 1) then
            --print("Creating class from single class...");
            setmetatable(class, {__index = select(1, ...)});

        -- The class will search for each method in the list of its parents
        -- 'arg' is the list of parents.    
        else
            local varargs = table.pack(...);
            setmetatable(class, {__index = 
                function(table, key)
                    local v = SearchForImplementation(key, varargs);
                    table[key] = v;
                    return v;
                end });
        end
    end

    -- prepare 'class' to be the metatable of its instances
    class.__index = class;

    -- Define a new Constructor for the WidgetScript class.
    function class:New(o)
        o = o or {};
        setmetatable(o, class);
        return o;
    end

    -- return the new Widget script
    return class;
end