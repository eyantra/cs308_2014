%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FbLoc Function 
% Input: image of the arena
% Output: Objects(Bounding Box, Centroid and Area) of blue connected components 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [stats]=FbLoc(data)

    % Now to track blue objects in real time we have to subtract the blue component
    % from the grayscale image to extract the blue components in the image
    diff_im = imsubtract(data(:,:,3), rgb2gray(data));

    % Use a median filter to filter out noise
    diff_im = medfilt2(diff_im, [3 3]);

    % Convert the resulting grayscale image into a binary image based on
    % threshold
    thresh =0.1;
    diff_im = (diff_im >= thresh * 255);

    % Remove all those components with less than 10 pixels
    diff_im = bwareaopen(diff_im,10);

    % Label all the connected components in the image
    bw = bwlabel(diff_im, 8);

    % Here we do the image blob analysis
    % We get a set of properties for each labeled region
    stats = regionprops(logical(bw), 'BoundingBox', 'Centroid', 'Area');

end