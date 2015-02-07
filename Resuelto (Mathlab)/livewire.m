function varargout = livewire(I, dRadius, dCaptureRadius)
%LIVEWIRE Select a region of interest with the help of a live-wire [1].
%   A replacement to MATLAB's ROIPOLY function. Supports ROIPOLY's
%   interactive modes, i.e. those syntaxes where no contour points X and Y
%   are supplied in the call. Also only supports the axes's genuine
%   coordinate system. This leaves the following syntaxes:
%
%   BW = LIVEWIRE(I, DRADIUS, DCAPTURERADIUS) displays the image I on the
%   screen and enables ineractive delineation of objects using the mouse.
%   Click the left mouse button to add anchor points to the path. Pressing
%   <BACKSPACE> or <DELETE> removes the latest anchor point.
%   A shift-click, right-click, or double-click adds a final anchor point
%   to the selection and then starts the fill; pressing <RETURN> finishes
%   the selection without adding an anchor point.
%   Parameter DRADIUS defines an aproximate ratdius (in pixels) around an
%   anchor point, within which the cheapest path to the anchor is
%   calculates. Large values of DRADIUS lead to slow behaviour of the GUI,
%   while supplying oly a limited livewire length. The default is 100.
%   Parameter DCAPTURERADIUS sets a radius around the mouse position (in
%   pixels) within which a better anchor point (situated on a strong image
%   gradient) is selected. This allows the user to be more sloppy in the
%   choice of anchor points, however, it may behave undesirably in some
%   situtations. Press and hold the <CTRL> button to disable this feature.
%
%   BW = LIVEWIRE(...) Same as above, but operates on the image in the
%   current axes.
%
%   [BW,xi,yi] = LIVEWIRE(...) returns the path coordinates in xi and yi.
%   Note that LIVEWIRE always produces a closed polygon (i.e. the
%   coordinates of the last point equal the coordinates of the first point).
%
%   If LIVEWIRE is called with no output arguments, the resulting image is
%   displayed in a new figure.
%
%   Example
%   -------
%       I = imread('cameraman.tif');
%       BW = livewire(I);
%       figure, imshow(BW)
%
%   IMPORTANT: Make sure to run the please_mex_me.m to compile the .cpp
%   file. Livewire will work without compiling this file, however, it will
%   be quite slow!
%
%   See also FLIVEWIREGETCOSTFCN, FLIVEWIRECALCP, FLIVEWIREGETPATH.
%
%
%   Copyright 2013 Christian Wïürslin, University of Tübingen and University
%   of Stuttgart, Germany. Contact: christian.wuerslin@med.uni-tuebingen.de
%
%
%   References:
%
%   [1] MORTENSEN, E. N.; BARRETT, W. A. Intelligent scissors for image
%   composition. In: SIGGRAPH '95: Proceedings of the 22nd annual
%   conference on Computer graphics and interactive techniques.
%   New York, NY, USA: ACM Press, 1995. p. 191:198.


% =========================================================================
% *** FUNCTION livewire
% ***
% *** Main GUI function. Creates the figure and all its contents and
% *** registers the callbacks.
% ***
% =========================================================================

dPointerCross = NaN(16);
dPointerCross(8   ,1:15) = 2;
dPointerCross(1:15,   8) = 2;

dPointerCrossMag = ...
   [NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ... 
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ... 
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
      2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN,   2, NaN, NaN, NaN,   2, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN,   2, NaN, NaN, NaN,   2, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN,   2, NaN, NaN, NaN,   2, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN,   2, NaN, NaN, NaN,   2, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN,   2, NaN, NaN, NaN,   2,   2,   2, NaN, NaN; ...
    NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN];

% -------------------------------------------------------------------------
% Check if number of output arguments is acceptable.
if nargout == 2 || nargout > 3
    error('LIVEWIRE expects either 0, 2 or 3 output arguments.'); 
end
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Check input argument and try to get a figure/axes/image handle if no
% input image is supplied (operate on current axes).
if ~nargin
    hF = get(0, 'CurrentFigure');
    if isempty(hF), error('Found no figure. Call LIVEWIRE with an image argument!'); end
    
    hA = get(hF, 'CurrentAxes');
    if isempty(hA), error('Found no axes. Call LIVEWIRE with an image argument!'); end
    
    hI = findobj(hA, 'Type', 'image');
    if isempty(hF), error('Found no image object. Call LIVEWIRE with an image argument!'); end
    
    dImg = double(get(hI, 'CData'));
else
    if size(I, 3) == 3, I = rgb2gray(I); end
    dImg = double(I);
    hI = imshow(dImg, []);
    hA = gca;
    hF = gcf;
end
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Define optional input parameters if omitted
if nargin < 2
    SOptions.dRadius = 200;
    SOptions.dCaptureRadius = 4;
end
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Bring figure to the front and set callbacks
figure(hF);
try
    set(hF, ...
        'WindowButtonDownFcn'   , @fButtonDownFcn,...
        'KeyPressFcn'           , @fKeyPressFcn, ...
        'KeyReleaseFcn'         , @fKeyReleaseFcn, ...
        'WindowButtonMotionFcn' , @fMotionFcn, ...
        'Pointer'               , 'custom', ...
        'PointerShapeCData'     , dPointerCrossMag, ...
        'PointerShapeHotSpot'   , [8, 8], ...
        'DoubleBuffer'          , 'on'); 
catch
    set(hF, ...
        'WindowButtonDownFcn'   , @fButtonDownFcn,...
        'KeyPressFcn'           , @fKeyPressFcn, ...
        'WindowButtonMotionFcn' , @fMotionFcn, ...
        'Pointer'               , 'custom', ...
        'PointerShapeCData'     , dPointerCrossMag, ...
        'PointerShapeHotSpot'   , [8, 8], ...
        'DoubleBuffer'          , 'on');
end
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Create the line objects (use two lines for better visibility)
hLine1 = line(...
    'Parent'    , hA, ...
    'XData'     , [], ...
    'YData'     , [], ...
    'Clipping'  , 'off', ...
    'Color'     , 'g', ...
    'LineStyle' , '-', ...
    'LineWidth' , 1.5);

hLine2 = line(...
    'Parent'    , hA, ...
    'XData'     , [], ...
    'YData'     , [], ...
    'Clipping'  , 'off', ...
    'Color'     , 'r', ...
    'LineStyle' , ':', ...
    'LineWidth' , 1.5);
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Initialize the global variables
dF          = fLiveWireGetCostFcn(dImg); % The cost function of the live-wire algorithm, see Ref [1].
iPX         = zeros(size(dImg), 'int8'); % The path map that shows the cheapest path to the sast anchor point.
iPY         = zeros(size(dImg), 'int8'); % The path map that shows the cheapest path to the sast anchor point.
dXData      = [];                        % The x-coordinates of the path
dYData      = [];                        % The y-coordinates of the path
iAnchorList = zeros(200, 1);             % A list of the anchor point indices in dXData and dYData for undo operations
iAnchorInd  = 0;                         % The index of the list
lRegularEnd = false;                     % Indicates whether path was successfully drawn or the UI was aborted.
lControl    = false;                     % Indicates whether the control key is pressed
% -------------------------------------------------------------------------

uiwait(hF);

% -------------------------------------------------------------------------
% ~~~~~~~~~~~~ A lot of user-friendly GUI interaction going on ~~~~~~~~~~~~
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Dialog was aborted: Seriously, who does that?
if ~lRegularEnd
    switch nargout

        case 1
            varargout{1} = false(size(dImg));

        case 3
            varargout{1} = false(size(dImg));
            varargout{2} = [];
            varargout{3} = [];

        otherwise

    end
    return
end
% -------------------------------------------------------------------------

% -------------------------------------------------------------------------
% Assign the return values
lMask = poly2mask(dXData, dYData, size(dImg, 1), size(dImg, 2));
switch nargout
    
    case 0
        figure, imshow(lMask)

    case 1
        varargout{1} = lMask;

    case 3
        varargout{1} = lMask;
        varargout{2} = dXData;
        varargout{3} = dYData;

    otherwise

end
% -------------------------------------------------------------------------

% =========================================================================
% The 'end' of the LIVEWIRE main function (real end is after the nested
% functions).
% =========================================================================




    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fButtonDownFcn (nested in livewire)
    % * * 
    % * * Figure callback
    % * *
    % * * Executed when the mouse button is pressed. Used to determine a
    % * * new anchor point or to end the user interaction.
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function fButtonDownFcn(hObject, eventdata) %#ok<*INUSD> eventdata is repeatedly unused
       
        % -----------------------------------------------------------------
        % Get mouse cursor position and return if outside the image
        [dX, dY] = fGetAxesPos;
        if ~dX, return, end
        % -----------------------------------------------------------------
        
        % -----------------------------------------------------------------
        % Look for ideal end-point within the capture radius
        if (SOptions.dCaptureRadius) && ~(lControl)
           [dX, dY] = fGetIdealAnchor(dX, dY, SOptions.dCaptureRadius);
        end
        % -----------------------------------------------------------------
        
        if isempty(dXData)
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            % The starting point of the path is selected
            dXData = dX;
            dYData = dY;
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        else
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            % A new anchor point and the cheapest path to the last anchor
            % point is appended to the path
            [iXPath, iYPath] = fLiveWireGetPath(iPX, iPY, dX, dY);
            if isempty(iXPath)
                iXPath = dX;
                iYPath = dY;
            end
            dXData = [dXData, double(iXPath(:)')];
            dYData = [dYData, double(iYPath(:)')];
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        end

        iAnchorInd = iAnchorInd + 1;
        iAnchorList(iAnchorInd) = length(dXData); % Save the previous path length for the undo operation
        
        % -----------------------------------------------------------------
        % Update the UI and calculate the new path map iP
        set([hLine1, hLine2], 'XData', dXData, 'YData', dYData);
        drawnow expose
        [iPX, iPY] = fLiveWireCalcP(dF, dX, dY, SOptions.dRadius);
        % -----------------------------------------------------------------

        % -----------------------------------------------------------------
        % If right-click, double-click or shift-click occurred, close path
        % and return.
        if ~(strcmp(get(hF, 'SelectionType'), 'normal')) && ~(lControl)
            [iXPath, iYPath] = fLiveWireGetPath(iPX, iPY, dXData(1), dYData(1));
            if isempty(iXPath)
                iXPath = dXData(1);
                iYPath = dYData(1);
            end
            dXData = [dXData, double(iXPath(:)')];
            dYData = [dYData, double(iYPath(:)')];
            set([hLine1, hLine2], 'XData', dXData, 'YData', dYData);
            drawnow expose
            set(hF, 'WindowButtonMotionFcn', '', 'WindowButtonDownFcn', '', 'KeyPressFcn', '');
            lRegularEnd = true;
            uiresume(hF);
        end

    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fButtonDownFcn
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    
    
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fMotionFcn (nested in livewire)
    % * * 
    % * * Figure callback
    % * *
    % * * Executes when the mouse is moved. Used to show, but not save, the
    % * * a preview of the currently cheapest path from the mouse pointer
    % * * to the last anchor point.
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function fMotionFcn(hObject, eventdata)
        
        % -----------------------------------------------------------------
        % Return if no start point has been selected yet or if the path map
        % is not yet available.
        if (isempty(dXData)) || (sum(abs(iPX(:))) == 0), return, end
        % -----------------------------------------------------------------
        
        % -----------------------------------------------------------------
        % Get mouse cursor position and return if outside of the image
        [dX, dY] = fGetAxesPos;
        if ~dX, return, end
        % -----------------------------------------------------------------
        
        % -----------------------------------------------------------------
        % Look for ideal end-point within the capture radius
        if (SOptions.dCaptureRadius) && ~(lControl)
           [dX, dY] = fGetIdealAnchor(dX, dY, SOptions.dCaptureRadius);
        end
        % -----------------------------------------------------------------
        
        % -----------------------------------------------------------------
        % Get the cheapest path from current cursor position to the last
        % anchor point and update UI, butr do not add to the path.
        [iXPath, iYPath] = fLiveWireGetPath(iPX, iPY, dX, dY);
        if isempty(iXPath)
            iXPath = dX;
            iYPath = dY;
        end
        set([hLine1, hLine2], 'XData', [dXData, double(iXPath(:)')], ...
                              'YData', [dYData, double(iYPath(:)')]);
        drawnow expose
        % ----------------------------------------------------------------
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fMotionFcn
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    
    
    
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fKeyPressFcn (nested in livewire)
    % * * 
    % * * Figure callback
    % * *
    % * * Keyboard key is pressed
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function fKeyPressFcn(hObject, eventdata)
        
        if strcmp(eventdata.Key, 'control')
            set(hF, 'PointerShapeCData', dPointerCross);
            lControl = true;
        end
       
        switch eventdata.Key
            
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            % ENTER: close the path and return
            case 'return'
                [iXPath, iYPath] = fLiveWireGetPath(iPX, iPY, dXData(1), dYData(1));
                if isempty(iXPath)
                    iXPath = dXData(1);
                    iYPath = dYData(1);
                end
                dXData = [dXData, double(iXPath(:)')];
                dYData = [dYData, double(iYPath(:)')];
                set([hLine1, hLine2], 'XData', dXData, 'YData', dYData);
                drawnow expose
                set(hF, 'WindowButtonMotionFcn', '', 'WindowButtonDownFcn', '', 'KeyPressFcn', '');
                lRegularEnd = true;
                uiresume(hF);
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            % DEL or BACKSPACE: delete path to last anchor
            case {'delete', 'backspace'}
                iAnchorInd = iAnchorInd - 1;
                if ~iAnchorInd
                    return
                end
                
                dXData = dXData(1:iAnchorList(iAnchorInd));
                dYData = dYData(1:iAnchorList(iAnchorInd));
                
                set([hLine1, hLine2], 'XData', dXData, 'YData', dYData);
                drawnow expose
                [iPX, iPY] = fLiveWireCalcP(dF, dXData(end), dYData(end), SOptions.dRadius);
                fMotionFcn(hObject, []);
            % - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            otherwise

        end
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fKeyPressFcn
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    
    
    
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fKeyReleaseFcn (nested in livewire)
    % * * 
    % * * Figure callback
    % * *
    % * * Executes keyboard key is realeased
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function fKeyReleaseFcn(hObject, eventdata)
        if strcmp(eventdata.Key, 'control')
            lControl = false;
            set(hF, 'PointerShapeCData', dPointerCrossMag);
        end
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fKeyReleaseFcn
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    
    
    
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fGetAxesPos (nested in livewire)
    % * * 
    % * * Returns the current mouse cursor poition in the axes coordinate
    % * * system. Returns [0, 0] if out of bounds.
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function [dX, dY] = fGetAxesPos()
        dPos  = get(hA, 'CurrentPoint');
        dXLim = get(hA, 'XLim');
        dYLim = get(hA, 'YLim');
        dX = dPos(1, 1);
        dY = dPos(1, 2);
        if (dX < dXLim(1)) || (dX > dXLim(2)) || ...
           (dY < dYLim(1)) || (dY > dYLim(2))
            dX = 0;
            dY = 0;
        end
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fGetAxesPos
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    
    
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fGetIdealAnchor (nested in livewire)
    % * * 
    % * * Tries to find an anchor point in the vicinity of (dX, dY)^T which
    % * * lies on an image gradient
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function [dX, dY] = fGetIdealAnchor(dX, dY, dRadius)
        iX = round(dX); iY = round(dY);
        if ~(iPX(iY, iX)) && ~(iPY(iY, iX)), return, end
        iInd = double(-round(dRadius):round(dRadius));
        iXVector = round(double(iX) + iInd.*double(iPX(iY, iX)));
        iYVector = round(double(iY) + iInd.*double(iPY(iY, iX)));
        lValid = (iXVector > 0) & (iXVector <= size(dF, 2)) & ...
                 (iYVector > 0) & (iYVector <= size(dF, 1));
        iXVector = iXVector(lValid); iYVector = iYVector(lValid);
        dVal = dF((iXVector - 1).*size(dF, 1) + iYVector - 1);
        [temp, iMinInd] = min(dVal); %#ok<ASGLU> // Backward compatibility
        dX = double(iXVector(iMinInd));
        dY = double(iYVector(iMinInd));
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fGetIdealAnchor
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    
    

    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * NESTED FUNCTION fCloseGUI (nested in livewire)
    % * * 
    % * * Figure callback
    % * *
    % * * Closes the figure if requested.
    % * *
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    function fCloseGUI(hObject, eventdata) %#ok<DEFNU> <-stupid!
        delete(hObject); % Bye-bye figure
    end
    % = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    % * * END NESTED FUNCTION fCloseGUI
	% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    
end
% =========================================================================
% *** END FUNCTION livewire (and its nested functions)
% =========================================================================