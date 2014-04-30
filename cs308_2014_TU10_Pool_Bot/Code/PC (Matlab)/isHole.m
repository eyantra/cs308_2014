function hole = isHole(x,y,epsilon,hx,hy)
h = 0;
for i=1:6
    if (abs(x-hx(i))<epsilon) && (abs(y-hy(i))<epsilon)
       h = 1; 
    end
end
hole = h;
end