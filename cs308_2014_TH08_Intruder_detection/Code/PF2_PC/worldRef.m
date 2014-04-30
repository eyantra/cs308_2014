%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WorldRef Function 
% Input: Pixel Coordinates (r, c)
% Output: Corresponding World Coordinates (x, y)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [ x, y ] = worldRef( r, c )
    min_points = 7;
    count=1;
    % Manually Taking 7 world coordinates from the arena
    im2 = [170, 150, 101, 60, 100, 30, 130; 130, 80, 30, 60, 100, 180, 150]';
    % Taking corresponding pixel coordinates of the 7 points taken above
    im1 = [493, 275, 120, 225, 351, 548, 540; 323, 252, 135, 85, 155, 78, 223]';

    for i=1:min_points
        % Populate the odd rows for A
        oddrow  = [ im1(i,1) im1(i,2) 1 0 0 0 -im1(i,1)*im2(i,1) -im1(i,2)*im2(i,1) ];
        % Populate the even rows for A
        evenrow = [ 0 0 0 im1(i,1) im1(i,2) 1 -im1(i,1)*im2(i,2) -im1(i,2)*im2(i,2) ];
        % Concatenate odd and even rows of A
        A(count*2-1,:)=oddrow;
        A(count*2,:)=evenrow;
        B(count*2-1) = im2(i,1);
        B(count*2) = im2(i,2);
        count=count+1;
    end
    B = B';
    
    % Computing H by taking pseudo inverse
    H = ((inv(A'*A))*(A'))*B;
    H(9) = 1;

    H1 = [H(1:3),H(4:6),H(7:9)]';
    A = [r,c,1]';
    U = H1*A;
    x = round(U(1)/U(3));
    y = round(U(2)/U(3));
end

