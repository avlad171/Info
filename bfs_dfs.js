var dy = [-1, 0, 0, 0, 0, 1];
var dx = [0, 1, 0, -1, 0, 0];
var dz = [0, 0, 1, 0, -1, 0];

var world;
var npc;

var coada2 = [];
function dfs (nivel,  x,  y, z)
{
    if(nivel > 15)
        return;
    
    
    for(var i = 0; i < 5; i++)
    {
        var newy  = y + dy[i];
        var newx = x + dx[i];
        var newz = z + dz[i];
        
        if(world.getBlock(newx, newy, newz).isAir())
        {
            world.setBlock(newx, newy, newz, \"minecraft:stone\", 0);
            coada2.push({posx: newx, posy: newy, posz: newz, nivel: nivel + 1});
            dfs(nivel + 1, newx, newy, newz);
        }
    }        
}

function bfs(x, y, z)
{
    var coada = [];
    var ops = 0;
    coada.push({posx: x, posy: y, posz: z, nivel: 0});
    
    while(coada.length > 0)
    {
        npc.say(\"OP\");
        ops ++;
        if(ops > 100)
            return;
            
        var curr = coada.shift();
        for(var i = 0; i < 5; i++)
        {
            var newy  = curr.posy + dy[i];
            var newx = curr.posx + dx[i];
            var newz = curr.posz + dz[i];
            var nivel = curr.nivel;
            
            if(world.getBlock(newx, newy, newz).isAir())
            {
                world.setBlock(newx, newy, newz, \"minecraft:stone\", 0);
                coada.push({posx: newx, posy: newy, posz: newz, nivel: nivel + 1});
            }
        }
    }
}
function tick (event)
{
npc = event.npc;
var x = npc.getX();
var y = npc.getY() - 1;
var z = npc.getZ();

//npc.say (\"Test\");

world = npc.getWorld();
dfs(0, x, y, z);

if(coada2.length > 0)
    {
    var curr = coada2.shift();
    world.setBlock(curr.posx, curr.posy, curr.posz, \"minecraft:wool\", curr.nivel % 16);
    }
}
